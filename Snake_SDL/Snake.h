#pragma once

#include <SDL.h>

#include "Texture.h"

#include <vector>

struct Position
{
	uint8_t x, y;
};

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
	SDL_Rect recApple;

	std::vector<SDL_Rect> recSnakeHeads;
	std::vector<SDL_Rect> recSnakeTails;
	std::vector<SDL_Rect> recSnakeBody;

	SDL_Event event;

	uint8_t apples = 0;

	Position applePos;

	Direction direction;

	const std::string pathToBackground = "resource/backgrund.jpg";
	const std::string pathToSprite = "resource/sprites.png";
	const std::string pathTograss = "resource/grass.jpg";

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;
	const int SPRITE_SIZE = 64;
	const int GAMEAREA_WIDTH = 12*SPRITE_SIZE;
	const int GAMEAREA_HEIGHT = 11*SPRITE_SIZE;

	bool quit = false;
	
	// Methods
	void keyPress(SDL_Keycode e);

	bool loadMedia(Texture &texture, const std::string &path) const;

	bool hitWall(Position nextPos) const;
	bool hitBody(Position nextPos) const;

	void hitApple() { ++apples; };

};



inline IMG_InitFlags getImageType(const std::string &path)
{
	auto extension = path.substr(path.find_last_of(".") + 1);

	if (extension == "png")
		return IMG_INIT_PNG;
	if (extension == "jpg")
		return IMG_INIT_JPG;
}