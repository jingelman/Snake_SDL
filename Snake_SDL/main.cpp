#include <stdio.h>

#include "Snake.h"

int main(int argc, char* args[])
{
	Snake snake;

	if (!snake.init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}

	if (!snake.loadMedia())
	{
		printf("Failed to load media!\n");
		return 0;
	}

	// Run game
	snake.eventLoop();

	return 0;
}
