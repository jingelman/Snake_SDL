#include "Snake.h"

#include "CoreManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "TimerManager.h"

#include "RandomGenerator.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

enum class Direction
{
	UP,
	RIGHT,
	LEFT,
	DOWN
} direction;

enum RENDERQUALITY
{
	NEAREST,
	LINEAR,
	ANISOTROPIC
};

Snake::Snake()
{
	backgroundArea = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	Uint16 heightBorder = 0; // 0.5*(SCREEN_HEIGHT - GAMEAREA_HEIGHT);
	Uint16 widthBorder = 0;// 0.1*SCREEN_HEIGHT;

	gameArea = { widthBorder, heightBorder, GAMEAREA_WIDTH, GAMEAREA_HEIGHT };

	HeadSprites.push_back({ 3 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	HeadSprites.push_back({ 4 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	HeadSprites.push_back({ 3 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	HeadSprites.push_back({ 4 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });

	TailSprites.push_back({ 3 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	TailSprites.push_back({ 4 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	TailSprites.push_back({ 3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	TailSprites.push_back({ 4 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });

	BodySprites.push_back({ 0 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	BodySprites.push_back({ 1 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	BodySprites.push_back({ 2 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	BodySprites.push_back({ 0 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	BodySprites.push_back({ 2 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	BodySprites.push_back({ 2 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });

	appleSprite = { 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };

}

Snake::~Snake()
{
	SoundManager::freeMusic();
	SoundManager::freeSoundEffect();
	TextureManager::freeTextures();
	CoreManager::freeCore();
}

bool Snake::init()
{
	bool success = true;

	TimerManager::startTimer();
	TimerManager::setFrameCap(true);

	if (!CoreManager::initVideo || !CoreManager::initAudio)
		success = false;
	else
	{
		CoreManager::setRenderQuality(LINEAR);

		if (!CoreManager::CreateWindow("Snake WoW!", SCREEN_WIDTH, SCREEN_HEIGHT))
			success = false;
		else
		{
			if (!CoreManager::CreateRenderer())
				success = false;
			else
			{
				CoreManager::SetRenderColor(255, 255, 255, 255);

				if (!TextureManager::initTexture())
					success = false;

				if (!SoundManager::initSound())
					success = false;
			}
		}
	}

	setupGame();

	return success;
}

bool Snake::loadMedia()
{
	bool success = true;

	if (!SoundManager::loadMusic(pathToMusic))
		success = false;

	if (!SoundManager::loadEffect(pathToEatEffect))
		success = false;

	if (!SoundManager::loadEffect(pathToLoseEffect))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathToBackground))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathToGrass))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathToSprite))
		success = false;
	
	if (!TextureManager::loadFont(pathToFont, 60))
		success = false;
		
	return success;
}

void Snake::setupGame()
{
	recSnake.clear();

	recSnake.push_back({ 3 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE });
	recSnake.push_back({ 2 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE });
	recSnake.push_back({ 1 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE });

	direction = Direction::RIGHT;

	appleCounter = 0;

	addApple();

	textPos = { (GAMEAREA_WIDTH - TextureManager::font.texture.mWidth) / 2, (GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight) / 3, TextureManager::font.texture.mWidth, TextureManager::font.texture.mHeight };

	gameSpeed = startSpeed;

}

void Snake::gameLoop()
{
	if (appleCounter > HighScore)
		HighScore = appleCounter;

	updatePos();
	updateSprites();
	render();
}

void Snake::eventLoop()
{
	bool quit = false;
	SDL_Event event;

	SoundManager::setMusicVolume(5);
	SoundManager::setEffectVolume(0, 30);
	SoundManager::setEffectVolume(1, 10);

	gameLoop();

	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0) {

			// x window button
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}

			if (event.type == SDL_KEYDOWN) {

				isStarted = true;

				auto key = event.key.keysym.sym;
				switch (key)
				{
				case SDLK_UP:
				case SDLK_w:
					if (direction != Direction::DOWN)
						direction = Direction::UP;// input = true;
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					if (direction != Direction::LEFT)
						direction = Direction::RIGHT;// input = true;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					if (direction != Direction::RIGHT)
						direction = Direction::LEFT;// input = true;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					if (direction != Direction::UP)
						direction = Direction::DOWN;// input = true;
					break;
				case SDLK_p:
					if (!TimerManager::isTimerStarted() && TimerManager::isTimerPaused())
					{
						TimerManager::unPauseTimer();
						SoundManager::resumeMusic();
					}
					else
					{
						TimerManager::pauseTimer();
						SoundManager::pauseMusic();
					}
					break;
				case SDLK_SPACE:
					if (hasLost)
					{
						isStarted = false;
						hasLost = false;
						SoundManager::pauseMusic();
						TimerManager::startTimer();
						setupGame();
						gameLoop();
					}
					break;
				case SDLK_ESCAPE: quit = true;
				default:
					break;
				}
			}
		}

		//If we want to cap the frame rate
		if (TimerManager::isFrameCap() && TimerManager::getTicks() < (Uint32)1000 / gameSpeed)
		{
			//Sleep the remaining frame time
			SDL_Delay(1000 / gameSpeed - TimerManager::getTicks());
		}

		if (TimerManager::isTimerStarted() && isStarted)
		{
			TimerManager::startTimer();

			if (!SoundManager::isMusicPlaying() || !SoundManager::isMusicPaused())
			{
				SoundManager::playMusic(0);
			}

			if (!TimerManager::isTimerPaused())
			{
				if (!hitWall(recSnake[0]) && !hitBody(recSnake))
				{
					gameLoop();
				}
				else
				{
					hasLost = true;

					TimerManager::stopTimer();
					SoundManager::pauseMusic();
					SoundManager::playEffect(1);

					// Render one extra frame to render text
					gameLoop();
#if _DEBUG
					printf("You ate %i apples!\n", getAppleCounter());
#endif
				}
			}
		}
	}
}

void Snake::updatePos()
{
	SDL_Rect temp;
	bool ateApple = false;

	switch (direction)
	{
	case Direction::UP:
#if _DEBUG
		printf("Going up!\n");
#endif
		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y -= TILE_SIZE;
		
		if (hitApple(recSnake[0]))
			ateApple = true;
		
		break;
	case Direction::RIGHT:
#if _DEBUG
		printf("Going right!\n");
#endif

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x += TILE_SIZE;

		if (hitApple(recSnake[0]))
			ateApple = true;

		break;
	case Direction::LEFT:
#if _DEBUG
		printf("Going left!\n");
#endif

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x -= TILE_SIZE;

		if (hitApple(recSnake[0]))
			ateApple = true;

		break;
	case Direction::DOWN:
#if _DEBUG
		printf("Going down!\n");
#endif

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y += TILE_SIZE;

		if (hitApple(recSnake[0]))
			ateApple = true;
		
		break;
	default:
		break;
	}

	if (ateApple)
	{
		SoundManager::playEffect(0);
		recSnake.push_back(temp);
		incrAppleCounter();
		addApple();
		if (appleCounter % 5 == 0)
			++gameSpeed;
	}

}

void Snake::updateSprites()
{
	tempPos.clear();
	tempPos.resize(recSnake.size());

	for (auto i = 0; i < recSnake.size(); ++i)
	{
		if (i == 0)
		{
			if (direction == Direction::UP)
				tempPos[i] = HeadSprites[0];
			else if (direction == Direction::RIGHT)
				tempPos[i] = HeadSprites[1];
			else if (direction == Direction::LEFT)
				tempPos[i] = HeadSprites[2];
			else
				tempPos[i] = HeadSprites[3];
		}
		else if (i == recSnake.size() - 1)
		{
			if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y) // up
				tempPos[i] = TailSprites[0];
			else if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y) // right
				tempPos[i] = TailSprites[1];
			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y) // left
				tempPos[i] = TailSprites[2];
			else
				tempPos[i] = TailSprites[3]; // down
		}
		else
		{
			if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y ||
				recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y)
				tempPos[i] = BodySprites[0];

			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y ||
				recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y)
				tempPos[i] = BodySprites[1];

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y ||
				recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y)
				tempPos[i] = BodySprites[2];

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y ||
				recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y)
				tempPos[i] = BodySprites[3];

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y ||
				recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y)
				tempPos[i] = BodySprites[4];

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y ||
				recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y)
				tempPos[i] = BodySprites[5];
		}
	}

}

void Snake::render()
{
	CoreManager::SetRenderColor(255, 255, 255, 255);
	CoreManager::clearRenderer();

	CoreManager::SetViewport(&backgroundArea);
	CoreManager::RenderCopy(TextureManager::getTexture(0));


	TextureManager::setText(CoreManager::getRenderer(), "High Score:", 0, 0, 0);
	textPos = { 49*(SCREEN_WIDTH - TextureManager::font.texture.mWidth) / 50, 2*(SCREEN_HEIGHT - TextureManager::font.texture.mHeight) / 50, TextureManager::font.texture.mWidth, TextureManager::font.texture.mHeight };
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);
	

	char snum[10];
	sprintf_s(snum, "%i", HighScore);
	TextureManager::setText(CoreManager::getRenderer(), snum, 0, 0, 0);

	textPos = { 40 * (SCREEN_WIDTH - TextureManager::font.texture.mWidth) / 50, 10 * (SCREEN_HEIGHT - TextureManager::font.texture.mHeight) / 50, 3*TextureManager::font.texture.mWidth, 3*TextureManager::font.texture.mHeight };
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);
	
	
	CoreManager::SetViewport(&gameArea);
	
	CoreManager::RenderCopy(TextureManager::getTexture(1));

	for (auto i = 0; i < tempPos.size(); ++i)
	{
		CoreManager::RenderCopy(TextureManager::getTexture(2), &tempPos[i], &recSnake[i]);
	}

	CoreManager::RenderCopy(TextureManager::getTexture(2), &appleSprite, &applePos);
	
	if (!isStarted && !hasLost)
	{
		TextureManager::setText(CoreManager::getRenderer(), "Press any key to start", 0, 0, 0);
		textPos = { (GAMEAREA_WIDTH  - TextureManager::font.texture.mWidth) / 2, (GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight) / 3, TextureManager::font.texture.mWidth, TextureManager::font.texture.mHeight };
		CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);

		textPos.y *= 1.5;

		TextureManager::setText(CoreManager::getRenderer(), "(Use the Arrow keys or WASD to to change direction)", 0, 0, 0);
		CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);
	}

	if (isStarted && hasLost)
	{
		//TODO inline finction
		char buf[20], snum[10];
		sprintf_s(snum, "%i", appleCounter);
		snprintf(buf, sizeof buf, "%s%s%s", "You ate ", snum, " apples!");

		TextureManager::setText(CoreManager::getRenderer(), buf, 0, 0, 0);

		textPos = { (GAMEAREA_WIDTH - TextureManager::font.texture.mWidth) / 2, (GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight) / 3, TextureManager::font.texture.mWidth, TextureManager::font.texture.mHeight };

		CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);

		TextureManager::setText(CoreManager::getRenderer(), "Press 'Spacebar' to play again", 0, 0, 0);

		textPos.y *= 1.5;

		CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &textPos);
	}


	CoreManager::SetViewport(&gameArea);
	
	CoreManager::render();
}

void Snake::addApple()
{
	bool freePos;

	SDL_Rect temp;

	do
	{
		freePos = true;

		Uint8 posX = randomNumber(0, tilesWidth - 1);
		Uint8 posY = randomNumber(0, tilesHeight - 1);

		temp = { posX * TILE_SIZE , posY * TILE_SIZE , TILE_SIZE, TILE_SIZE };

		for (auto p : recSnake)
		{
			if (p.x == temp.x && p.y == temp.y)
				freePos = false;
		}
	} while (!freePos);

	applePos = temp;
}

bool Snake::hitApple(const SDL_Rect& head) const
{
	return head.x == applePos.x && head.y == applePos.y;
}

bool Snake::hitWall(const SDL_Rect& head) const
{
	bool hit = false;

	if (head.x < 0 || head.y < 0 || head.x + head.w > GAMEAREA_WIDTH || head.y + head.h > GAMEAREA_HEIGHT)
	{
#if _DEBUG
		printf("You hit the wall! \n");
#endif
		hit = true;
	}

	return hit;
}

bool Snake::hitBody(const std::vector<SDL_Rect>& snake)
{
	bool hit = false;
	const auto& head = snake[0];

	for (auto p = 1; p < recSnake.size() - 1; ++p)
	{
		auto& snakePart = recSnake[p];
		if (snakePart.x == head.x && snakePart.y == head.y)
		{
#if _DEBUG
			printf("You hit your own body! \n");
#endif
			hit = true;
		}
	}

	return hit;
}
