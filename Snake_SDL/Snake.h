#pragma once

#include <SDL.h>
#include <vector>

#include "Texture.h"
#include "Timer.h"
#include "Music.h"
#include "soundEffect.h"


enum class Direction
{
	UP,
	RIGHT,
	LEFT,
	DOWN
};

class Snake
{
public:
	Snake();
	~Snake();

	bool init();
	void setupGame();
	void render();
	bool loadMedia_new();

	void gameLoop();
	void updatePos(Direction dir);

private:

	//The window we'll be rendering to
	SDL_Window* mWindow = nullptr;

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

	Music music;
	SoundEffect appleEffect;
	SoundEffect loseEffect;

	uint8_t apples = 0;

	Direction direction;
	Timer timer;
	Uint32 countedFrames = 0;

	const char* pathToBackground = "resource/texture/backgrund.jpg";
	const char* pathToSprite = "resource/texture/sprites.png";
	const char* pathTograss = "resource/texture/grass.jpg";

	const char* pathToMusic = "resource/sound/369920__mrthenoronha__cartoon-game-theme-loop.wav";
	const char* pathToLoseEffect = "resource/sound/362375__shapingwaves__sw001-8-bit-games-music-element-fail-01-arp.wav";
	const char* pathToEatEffect = "resource/sound/20265__koops__apple-crunch-02.wav";

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 800;
	const int SPRITE_SIZE = 64;
	const int GAMEAREA_WIDTH = 12 * SPRITE_SIZE;
	const int GAMEAREA_HEIGHT = 11 * SPRITE_SIZE;

	bool quit = false;

	// Methods
	void keyPress(SDL_Keycode e);

	SDL_Rect addApple();
	bool hitWall(const SDL_Rect& head);
	bool Snake::hitBody(const std::vector<SDL_Rect>& snake);

	bool hitApple(const int x, const int y) const;
};
