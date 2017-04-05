#include "Snake.h"

#include <SDL_image.h>
#include <stdio.h>
#include <random>

std::default_random_engine generator;
std::uniform_int_distribution<int> distributionX(0, 11);
std::uniform_int_distribution<int> distributionY(0, 10);

Snake::Snake()
{
	int border = 50;

	recGameArea = {border, border, GAMEAREA_WIDTH, GAMEAREA_HEIGHT};

	recBackground = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

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

	recSnake.push_back({5 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnake.push_back({4 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});
	recSnake.push_back({3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE});

	direction = Direction::RIGHT;

	recApple_pos = addApple();
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

	IMG_Quit();
	SDL_Quit();
}


bool Snake::init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
			}
		}
	}

	return success;
}

void Snake::gameLoop()
{
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				auto key = event.key.keysym.sym;
				keyPress(key);
			}
		}

		//Clear screen
		SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(mRenderer);

		SDL_RenderSetViewport(mRenderer, &recBackground);


		//Render background to screen
		SDL_RenderCopy(mRenderer, texBackground.mTexture, NULL, NULL);

		SDL_RenderSetViewport(mRenderer, &recGameArea);


		//Render grass to screen
		SDL_RenderCopy(mRenderer, texGrass.mTexture, NULL, NULL);

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

		{
			SDL_RenderCopy(mRenderer, texSprites.mTexture, &recApple, &recApple_pos);
		}

		SDL_RenderSetViewport(mRenderer, &recGameArea);

		//Update screen
		SDL_RenderPresent(mRenderer);
	}
}


void Snake::keyPress(SDL_Keycode e)
{
	switch (e)
	{
	case SDLK_ESCAPE:
		quit = true;
		break;
	case SDLK_UP:
		auto isSameDirection = recSnake[0];
		isSameDirection.y -= SPRITE_SIZE;

		if (isSameDirection.x == recSnake[1].x && isSameDirection.y == recSnake[1].y)
			break;

		auto temp = recSnake[recSnake.size() - 1];

		printf("Going up!\n");
		direction = Direction::UP;

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y -= SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			recApple_pos = addApple();
			//recSnake.insert(recSnake.begin(), { recSnake[0].x, recSnake[0].y, SPRITE_SIZE, SPRITE_SIZE });
			recSnake.push_back(temp);
		}

		break;
	case SDLK_RIGHT:
		isSameDirection = recSnake[0];
		isSameDirection.x += SPRITE_SIZE;

		if (isSameDirection.x == recSnake[1].x && isSameDirection.y == recSnake[1].y)
			break;

		temp = recSnake[recSnake.size() - 1];

		printf("Going right!\n");
		direction = Direction::RIGHT;

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x += SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case SDLK_LEFT:
		isSameDirection = recSnake[0];
		isSameDirection.x -= SPRITE_SIZE;

		if (isSameDirection.x == recSnake[1].x && isSameDirection.y == recSnake[1].y)
			break;

		temp = recSnake[recSnake.size() - 1];

		printf("Going left!\n");
		direction = Direction::LEFT;

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].x -= SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

		break;
	case SDLK_DOWN:
		isSameDirection = recSnake[0];
		isSameDirection.y += SPRITE_SIZE;

		if (isSameDirection.x == recSnake[1].x && isSameDirection.y == recSnake[1].y)
			break;

		temp = recSnake[recSnake.size() - 1];

		printf("Going down!\n");
		direction = Direction::DOWN;

		for (int i = recSnake.size() - 1; i > 0; --i)
		{
			recSnake[i] = recSnake[i - 1];
		}
		recSnake[0].y += SPRITE_SIZE;

		if (hitApple(recSnake[0].x, recSnake[0].y))
		{
			recApple_pos = addApple();
			recSnake.push_back(temp);
		}

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

SDL_Rect Snake::addApple()
{
	bool freePos;

	SDL_Rect temp;


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
