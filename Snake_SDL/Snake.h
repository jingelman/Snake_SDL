#pragma once

#include <SDL.h>
#include <vector>

class Snake
{
public:
	Snake();
	~Snake();

	bool init();
	bool loadMedia();
	void gameLoop();
	
private: // Methods

	void setupGame();
	void updatePos();
	void updateSprites();
	void render();

	SDL_Rect addApple();
	bool hitWall(const SDL_Rect& head) const;
	bool hitApple(const SDL_Rect& head) const;
	bool Snake::hitBody(const std::vector<SDL_Rect>& snake);

private: // Variables

	SDL_Rect backgroundArea;
	SDL_Rect gameArea;
	SDL_Rect applePos;
	SDL_Rect appleSprite;

	std::vector<SDL_Rect> recSnake;
	std::vector<SDL_Rect> tempPos;
	std::vector<SDL_Rect> HeadSprites;
	std::vector<SDL_Rect> TailSprites;
	std::vector<SDL_Rect> BodySprites;

	const char* pathToBackground = "resource/texture/backgrund.jpg";
	const char* pathToSprite = "resource/texture/sprites.png";
	const char* pathTograss = "resource/texture/grass.jpg";

	const char* pathToMusic = "resource/sound/369920__mrthenoronha__cartoon-game-theme-loop.wav";
	const char* pathToLoseEffect = "resource/sound/333785__projectsu012__8-bit-failure-sound.wav";
	const char* pathToEatEffect = "resource/sound/20265__koops__apple-crunch-02.wav";

	const Uint16 TILE_SIZE = 32;
	const Uint16 SPRITE_SIZE = 64;

	const Uint16 SCREEN_WIDTH = 1280;
	const Uint16 SCREEN_HEIGHT = 800;

	const Uint16 tilesWidth = 12;
	const Uint16 tilesHeight = 12;

	const Uint16 GAMEAREA_WIDTH = tilesWidth * SPRITE_SIZE;
	const Uint16 GAMEAREA_HEIGHT = tilesHeight * SPRITE_SIZE;

};
