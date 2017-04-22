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

	if (!snake.load_media())
	{
		printf("Failed to load media!\n");
		return 0;
	}

	// Run game
	snake.game_loop();

	return 0;
}
