#pragma once

#include <SDL.h>

#include "Texture.h"

#include <vector>

struct Position
{
	uint8_t x, y;
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
	SDL_Window *gwindow = nullptr;

	//The window renderer
	SDL_Renderer* gRenderer = nullptr;

	//Current displayed texture
	Texture gTexture;

	SDL_Rect stretchBackground;
	SDL_Rect gameAreaBackground;

	SDL_Event event;



	std::vector<Position> snakeBody;

	uint8_t apples = 0;

	Position applePos;

	const std::string path = "resource/backgrund.jpg";

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;

	bool quit = false;
	
	// Methods
	void keyPress(SDL_Keycode e);

	bool loadMedia(const std::string &path);

	bool hitWall(Position nextPos) const;
	bool hitBody(Position nextPos) const;

	void hitApple() { ++apples; };

};
