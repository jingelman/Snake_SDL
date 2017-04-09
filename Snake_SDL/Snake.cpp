#include "Snake.h"

#include <SDL_image.h>
#include "SoundEffect.h"
#include "Music.h"

#include <stdio.h>
#include <random>

std::uniform_int_distribution<int> distributionX(0, 11);
std::uniform_int_distribution<int> distributionY(0, 10);

#define SLOW_FRAMERATE 7
#define MEDIUM_FRAMERATE 10
#define FAST_FRAMERATE 13

Snake::Snake()
{
	recBackground = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	int heightBorder = (SCREEN_HEIGHT - GAMEAREA_HEIGHT) / 2;
	int widthBorder = SCREEN_HEIGHT * 0.1;

	recGameArea = {widthBorder, heightBorder, GAMEAREA_WIDTH, GAMEAREA_HEIGHT};

	recSnakeHeads.push_back({3 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeHeads.push_back({4 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeHeads.push_back({3 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeHeads.push_back({4 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});

	recSnakeTails.push_back({3 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeTails.push_back({4 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeTails.push_back({3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeTails.push_back({4 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});

	recSnakeBody.push_back({0 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeBody.push_back({1 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeBody.push_back({2 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeBody.push_back({0 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeBody.push_back({2 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnakeBody.push_back({2 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});

	recApple = {0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
}

Snake::~Snake()
{
	SDL_DestroyTexture(texSprites.mTexture);
	texSprites.mTexture = nullptr;

	SDL_DestroyTexture(texBackground.mTexture);
	texBackground.mTexture = nullptr;

	SDL_DestroyTexture(texGrass.mTexture);
	texGrass.mTexture = nullptr;

	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;

	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	freeMusic(music);
	freeSoundEffect(appleEffect);
	freeSoundEffect(loseEffect);

	IMG_Quit();
	SDL_Quit();
}


bool Snake::init()
{
	bool success = true;

	startTimer(timer);
	setFrameCap(timer, true);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)  < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		mWindow = SDL_CreateWindow("Snake WoW!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (mWindow == nullptr)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
			if (mRenderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = getImageType(pathToBackground);
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Load media
				if (!loadMedia(texBackground, pathToBackground))
				{
					printf("Failed to load media!\n");
				}

				imgFlags = getImageType(pathTograss);
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (!loadMedia(texGrass, pathTograss))
				{
					printf("Failed to load media!\n");
				}

				imgFlags = getImageType(pathToSprite);
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (!loadMedia(texSprites, pathToSprite))
				{
					printf("Failed to load media!\n");
				}

				initSound();
			}
		}
	}

	setupGame();

	return success;
}

void Snake::setupGame()
{
	recSnake.clear();

	recSnake.push_back({3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnake.push_back({2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnake.push_back({1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});

	direction = Direction::RIGHT;

	recApple_pos = addApple();
}


void Snake::gameLoop()
{
	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_KEYDOWN)
			{
				keyPress(event.key.keysym.sym);
			}
		}

		//If we want to cap the frame rate
		if (isFrameCap(timer) && getTicks(timer) < (Uint32)1000 / MEDIUM_FRAMERATE)
		{
			//Sleep the remaining frame time
			SDL_Delay(1000 / MEDIUM_FRAMERATE - getTicks(timer));
		}

		if(isTimerStarted(timer))
		{
			startTimer(timer);

			if (!isMusicPlaying() || !isMusicPaused())
			{
				playMusic(music);
			}

			if (!isTimerPaused(timer))
			{
				if(!hitWall(recSnake[0]) && !hitBody(recSnake))
				{
					move(direction);
					render();
				}
				else
				{
					stopTimer(timer);
					Mix_PauseMusic();
				}				
			}
		}

		++countedFrames;
	}
}

void Snake::render()
{
	//Clear screen
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderClear(mRenderer);
	SDL_RenderSetViewport(mRenderer, &recBackground);

	SDL_RenderCopy(mRenderer, texBackground.mTexture, nullptr, nullptr);
	SDL_RenderSetViewport(mRenderer, &recGameArea);
	
	SDL_RenderCopy(mRenderer, texGrass.mTexture, nullptr, nullptr);

	// Render snake
	for (int i = 0; i < recSnake.size(); ++i)
	{
		if (i == 0)
		{
			if (direction == Direction::UP)
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeHeads[0], &recSnake[i]);
			else if (direction == Direction::RIGHT)
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeHeads[1], &recSnake[i]);
			else if (direction == Direction::LEFT)
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeHeads[2], &recSnake[i]);
			else
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeHeads[3], &recSnake[i]);
		}
		else if (i == recSnake.size() - 1)
		{
			if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y) // up
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeTails[0], &recSnake[i]);
			else if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y) // right
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeTails[1], &recSnake[i]);
			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y) // left
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeTails[2], &recSnake[i]);
			else
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeTails[3], &recSnake[i]); // down
		}
		else
		{
			//TODO check direction
			if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // left-up
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[3], &recSnake[i]);

			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // left-left
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[1], &recSnake[i]);

			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y) // up-left
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[2], &recSnake[i]);

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y) // up-up
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[4], &recSnake[i]);

			else if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y < recSnake[i + 1].y) // up-right
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[0], &recSnake[i]);

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x < recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // left-down
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[0], &recSnake[i]);

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y) // down-down
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[4], &recSnake[i]);

			else if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // right-right
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[1], &recSnake[i]);

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y < recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // down-right
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[2], &recSnake[i]);

			else if (recSnake[i].x == recSnake[i - 1].x && recSnake[i].y > recSnake[i - 1].y && recSnake[i].x > recSnake[i + 1].x && recSnake[i].y == recSnake[i + 1].y) // right-up
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[5], &recSnake[i]);

			else if (recSnake[i].x < recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y) // down-right
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[3], &recSnake[i]);

			else if (recSnake[i].x > recSnake[i - 1].x && recSnake[i].y == recSnake[i - 1].y && recSnake[i].x == recSnake[i + 1].x && recSnake[i].y > recSnake[i + 1].y) // down-left
				SDL_RenderCopy(mRenderer, texSprites.mTexture, &recSnakeBody[5], &recSnake[i]);
		}
	}
	
	SDL_RenderCopy(mRenderer, texSprites.mTexture, &recApple, &recApple_pos);
	SDL_RenderSetViewport(mRenderer, &recGameArea);

	//Update screen
	SDL_RenderPresent(mRenderer);
}

void Snake::move(Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		printf("Going up!\n");

		auto temp = recSnake[recSnake.size() - 1];

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y -= SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			playEffect(appleEffect);
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::RIGHT:
		printf("Going right!\n");

		temp = recSnake[recSnake.size() - 1];

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x += SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			playEffect(appleEffect);
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::LEFT:
		printf("Going left!\n");

		temp = recSnake[recSnake.size() - 1];

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x -= SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			playEffect(appleEffect);
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case Direction::DOWN:
		printf("Going down!\n");

		temp = recSnake[recSnake.size() - 1];

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y += SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			playEffect(appleEffect);
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	default:
		break;
	}


}

void Snake::keyPress(SDL_Keycode e)
{
	switch (e)
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
		if (!isTimerStarted(timer) && isTimerPaused(timer))
		{
			unPauseTimer(timer); 
			Mix_ResumeMusic();
		}
		else
		{
			pauseTimer(timer); 
			Mix_PauseMusic();
		}
		break;
	case SDLK_SPACE:
		startTimer(timer);
		setupGame();
		break;
	case SDLK_ESCAPE:
		quit = true;
		break;
	default:
		break;
	}
}

bool Snake::loadMedia(Texture& texture, const std::string& path) const
{
	bool success = true;

	loadFromFile(texture, mRenderer, path);
	if (texture.mTexture == nullptr)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		success = false;
	}

	return success;
}

bool Snake::loadMedia_new()
{
	bool success = true;

	if (!loadMusic(music, pathToMusic))
		success = false;

	if (!loadEffect(appleEffect, pathToEatEffect))
		success = false;

	//if (!loadEffect(loseEffect, pathToLoseEffect))
	//	success = false;

	return success;
}

SDL_Rect Snake::addApple()
{
	bool freePos;

	SDL_Rect temp;

	std::random_device rd; //Will be used to obtain a seed for the random number engine
	std::mt19937 generator(rd());

	do
	{
		freePos = true;

		int xPos = distributionX(generator);
		int yPos = distributionY(generator);

		temp = {xPos * SPRITE_SIZE , yPos * SPRITE_SIZE , SPRITE_SIZE, SPRITE_SIZE};

		for (auto p : recSnake)
		{
			if (p.x == temp.x && p.y == temp.y)
				freePos = false;
		}
	}
	while (!freePos);


	return temp;
}

bool Snake::hitApple(const int x, const int y) const
{
	return x == recApple_pos.x && y == recApple_pos.y;
}


bool Snake::hitWall(const SDL_Rect& head)
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
