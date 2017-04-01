#include <SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* args[])
{
	
	// Window to be rendered in
	SDL_Window *window = NULL;

	// 2d image
	SDL_Surface* screenSurface = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("SDL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == nullptr)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get screen surface
			screenSurface = SDL_GetWindowSurface(window);

			// Fill with color (white)
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			// Uptdate screen
			SDL_UpdateWindowSurface(window);

			// Wait 2 sec
			SDL_Delay(2000);
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}