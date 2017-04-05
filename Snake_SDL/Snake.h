#pragma once

#include <SDL.h>

#include "Texture.h"

#include <vector>


enum class Direction
{
	UP, RIGHT, LEFT, DOWN
};

class Snake
{
public:
	Snake();
	~Snake();

	bool init();

	void gameLoop();

private:

	//The window we'll be rendering to
	SDL_Window *mWindow = nullptr;

	//The window renderer
	SDL_Renderer* mRenderer = nullptr;

	//Current displayed texture
	Texture texBackground;
	Texture texSprites;
	Texture texGrass;

	SDL_Rect recBackground;
	SDL_Rect recGameArea;
	std::vector<SDL_Rect> recSnake;
	SDL_Rect recApple, recApple_pos;

	std::vector<SDL_Rect> recSnakeHeads;
	std::vector<SDL_Rect> recSnakeTails;
	std::vector<SDL_Rect> recSnakeBody;

	SDL_Event event;

	uint8_t apples = 0;

	Direction direction;

	const char* pathToBackground = "resource/backgrund.jpg";
	const char* pathToSprite = "resource/sprites.png";
	const char* pathTograss = "resource/grass.jpg";

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;
	const int SPRITE_SIZE = 64;
	const int GAMEAREA_WIDTH = 12*SPRITE_SIZE;
	const int GAMEAREA_HEIGHT = 11*SPRITE_SIZE;

	bool quit = false;
	
	// Methods
	void keyPress(SDL_Keycode e);

	bool loadMedia(Texture &texture, const std::string &path) const;

	SDL_Rect addApple();

	bool hitApple(const int x, const int y) const;

};

inline IMG_InitFlags getImageType(const char* path)
{
	int index = -1;
	for(int i = strlen(path) - 1; i >= 0  ; --i)
	{
		if (path[i] == '.')
		{
			index = i;
			break;
		}
	}

	if (path + index == "png")
		return IMG_INIT_PNG;
	if (path + index == "jpg")
		return IMG_INIT_JPG;
}