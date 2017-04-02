#include "Snake.h"

#include <SDL_image.h>
#include <stdio.h>

Snake::Snake()
{
	gameAreaBackground = { SCREEN_WIDTH / 10, SCREEN_HEIGHT / 15, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

	stretchBackground = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	snakeBody.reserve(SCREEN_WIDTH*SCREEN_HEIGHT);
}


Snake::~Snake()
{
	SDL_DestroyTexture(gTexture.mTexture);
	gTexture.mTexture = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_DestroyWindow(gwindow);
	gwindow = nullptr;

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

		gwindow = SDL_CreateWindow("Snake WoW!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gwindow == nullptr)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}		//Load media
			if (!loadMedia(path))
			{
				printf("Failed to load media!\n");
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
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		SDL_RenderSetViewport(gRenderer, &stretchBackground);

		//Render texture to screen
		SDL_RenderCopy(gRenderer, gTexture.mTexture, NULL, NULL);

		SDL_RenderSetViewport(gRenderer, &gameAreaBackground);

		//Render texture to screen
		SDL_RenderCopy(gRenderer, gTexture.mTexture, NULL, NULL);

		//Update screen
		SDL_RenderPresent(gRenderer);
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
		printf("Going up!\n");
		break;
	case SDLK_DOWN:
		printf("Going down!\n");
		break;
	case SDLK_LEFT:
		printf("Going left!\n");
		break;
	case SDLK_RIGHT:
		printf("Going right!\n");
		break;
	default:
		break;
	}
}


bool Snake::loadMedia(const std::string &path)
{
	bool success = true;

	loadFromFile(gTexture, gRenderer, path);
	if (gTexture.mTexture == nullptr)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		success = false;
	}
	
	return success;
}

bool Snake::hitWall(Position nextPos) const
{
	if (nextPos.x >= 0 && (nextPos.y == 0 || nextPos.y == SCREEN_HEIGHT) || nextPos.y >= 0 && (nextPos.x == 0 || nextPos.x == SCREEN_WIDTH))
		return true;

	return false;
}


bool Snake::hitBody(Position nextPos) const
{
	for(const auto &pos : snakeBody)
	{
		if (pos.x == nextPos.x && pos.y == nextPos.y)
			return true;
	}
	
	return false;
}