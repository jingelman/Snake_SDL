#include <stdio.h>

#include "Snake.h"

int main(int argc, char* args[])
{
	
	Snake snake;

	if(!snake.init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}
	else
	{
		if (!snake.loadMedia_new())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			snake.gameLoop();
		}
	}


	return 0;
}