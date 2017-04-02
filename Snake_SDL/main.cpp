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

	snake.gameLoop();

	return 0;
}