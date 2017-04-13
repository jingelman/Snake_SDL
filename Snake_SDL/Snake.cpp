#include "Snake.h"

#include "CoreManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "TimerManager.h"

#include "RandomGenerator.h"

#include <stdio.h>

enum class Direction
{
	UP,
	RIGHT,
	LEFT,
	DOWN
} direction;

enum GAMESPEED
{
	SLOW_FRAMERATE = 7,
	MEDIUM_FRAMERATE = 10,
	FAST_FRAMERATE = 13
} speed;

enum RENDERQUALITY
{
	NEAREST,
	LINEAR,
	ANISOTROPIC
};

Snake::Snake()
{
	backgroundArea = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	Uint16 heightBorder = 0.5*(SCREEN_HEIGHT - GAMEAREA_HEIGHT);
	Uint16 widthBorder = 0.1*SCREEN_HEIGHT;

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
	TextureManager::freeTexture();
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

	/*if (!loadEffect(loseEffect, pathToLoseEffect))
	success = false;
	*/

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathToBackground))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathTograss))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), pathToSprite))
		success = false;


	return success;
}

void Snake::setupGame()
{
	recSnake.clear();

	recSnake.push_back({ 3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	recSnake.push_back({ 2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });
	recSnake.push_back({ 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE });

	direction = Direction::RIGHT;

	applePos = addApple();

	speed = GAMESPEED::MEDIUM_FRAMERATE;
}

void Snake::gameLoop()
{
	bool quit = false;
	Uint32 countedFrames = 0;
	SDL_Event event;

	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					if (direction != Direction::DOWN)
						direction = Direction::UP;
					break;
				case SDLK_RIGHT:
					if (direction != Direction::LEFT)
						direction = Direction::RIGHT;
					break;
				case SDLK_LEFT:
					if (direction != Direction::RIGHT)
						direction = Direction::LEFT;
					break;
				case SDLK_DOWN:
					if (direction != Direction::UP)
						direction = Direction::DOWN;
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
				case SDLK_j:
				{
					speed = GAMESPEED::SLOW_FRAMERATE;
					break;
				}
				case SDLK_k:
				{
					speed = GAMESPEED::MEDIUM_FRAMERATE;
					break;
				}
				case SDLK_l:
				{
					speed = GAMESPEED::FAST_FRAMERATE;
					break;
				}
				case SDLK_SPACE:
					TimerManager::startTimer();
					setupGame();
					break;
				case SDLK_ESCAPE: quit = true;
				default:
					break;
				}
			}
		}

		//If we want to cap the frame rate
		if (TimerManager::isFrameCap() && TimerManager::getTicks() < (Uint32)1000 / speed)
		{
			//Sleep the remaining frame time
			SDL_Delay(1000 / speed - TimerManager::getTicks());
		}

		if (TimerManager::isTimerStarted())
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
					updatePos();
					updateSprites();
					render();
				}
				else
				{
					TimerManager::stopTimer();
					SoundManager::pauseMusic();
				}
			}
		}
		++countedFrames;
	}
}

void Snake::updatePos()
{
	switch (direction)
	{
	case Direction::UP:
		printf("Going up!\n");

		auto temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y -= SPRITE_SIZE;

		if (hitApple(recSnake[0]))
		{
			SoundManager::playEffect(0);
			applePos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::RIGHT:
		printf("Going right!\n");

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x += SPRITE_SIZE;

		if (hitApple(recSnake[0]))
		{
			SoundManager::playEffect(0);
			applePos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::LEFT:
		printf("Going left!\n");

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x -= SPRITE_SIZE;

		if (hitApple(recSnake[0]))
		{
			SoundManager::playEffect(0);
			applePos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::DOWN:
		printf("Going down!\n");

		temp = recSnake[recSnake.size() - 1];

		for (auto i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y += SPRITE_SIZE;

		if (hitApple(recSnake[0]))
		{
			SoundManager::playEffect(0);
			applePos = addApple();
			recSnake.push_back(temp);
		}

		break;
	default:
		break;
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

	CoreManager::RenderCopy(TextureManager::textures[0]);
	CoreManager::SetViewport(&gameArea);

	CoreManager::RenderCopy(TextureManager::textures[1]);

	for (auto i = 0; i < tempPos.size(); ++i)
	{
		CoreManager::RenderCopy(TextureManager::textures[2], &tempPos[i], &recSnake[i]);
	}

	CoreManager::RenderCopy(TextureManager::textures[2], &appleSprite, &applePos);
	CoreManager::SetViewport(&gameArea);

	CoreManager::render();
}

SDL_Rect Snake::addApple()
{
	bool freePos;

	SDL_Rect temp;

	do
	{
		freePos = true;

		Uint8 posX = randomNumber(0, 11);
		Uint8 posY = randomNumber(0, 10);

		temp = { posX * SPRITE_SIZE , posY * SPRITE_SIZE , SPRITE_SIZE, SPRITE_SIZE };

		for (auto p : recSnake)
		{
			if (p.x == temp.x && p.y == temp.y)
				freePos = false;
		}
	} while (!freePos);

	return temp;
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
		printf("You hit the wall! \n");
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
			printf("You hit your own body! \n");
			hit = true;
		}
	}

	return hit;
}
