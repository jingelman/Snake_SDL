#pragma once

#include <SDL.h>
#include <vector>

class Snake
{
public:
	Snake();
	~Snake();

	bool init();
	bool load_media();
	void game_loop();

private: // Methods

	void setup_game();

	void event_loop();

	void update_apple_position();
	void update_snake_position();
	void update_sprites();
	void update_simulation(float dt);

	void add_apple();

	bool hit_wall(const SDL_Rect& head) const;
	bool hit_apple(const SDL_Rect& head) const;
	bool hit_object(const SDL_Rect &object, bool head);

	void render_pass_background();
	void render_pass_gamearea();
	void render_pass_instruction_text();
	void render_pass_start_text();
	void render_pass_lost_text();
	void render_pass_highscore_text();
	void render_pass_snake();

	void render();

private: // Variables

	bool quit;
	bool m_hasLost = false;
	bool m_isPaused;
	bool first_game = true;
	bool double_speed = false;
	bool render_intro_text;
	bool render_high_score;

	const float C_SPEEDUP = 1.5f;
	const float C_START_SPEED = 320.f;
	float velocity = C_START_SPEED;

	uint8_t apple_counter = 0;
	uint8_t highscore_counter = 0;

	SDL_Rect background_area;
	SDL_Rect game_area;
	SDL_Rect apple_position;
	SDL_Rect apple_sprite;
	SDL_Rect text_position;

	std::vector<SDL_Rect> rec_snake;
	std::vector<SDL_Rect> temp_position;
	std::vector<SDL_Rect> head_sprites;
	std::vector<SDL_Rect> tail_sprites;
	std::vector<SDL_Rect> body_sprites;

	const uint16_t C_TILE_SIZE = 32;
	const uint16_t C_SPRITE_SIZE = 64;

	const uint16_t C_SCREEN_WIDTH = 1280;
	const uint16_t C_SCREEN_HEIGHT = 800;

	const uint16_t C_TILE_WIDTH = 28;
	const uint16_t C_TILE_HEIGHT = 25;

	const uint16_t C_GAMEAREA_WIDTH = C_TILE_WIDTH * C_TILE_SIZE;
	const uint16_t C_GAMEAREA_HEIGHT = C_SCREEN_HEIGHT;

	const char* path_to_font = "resource/font/BADABB__.ttf";
	const char* path_to_background_texture = "resource/texture/backgrund.jpg";
	const char* path_to_sprite_texture = "resource/texture/sprites.png";
	const char* path_to_grass_texture = "resource/texture/grass.jpg";
	const char* path_to_music = "resource/sound/369920__mrthenoronha__cartoon-game-theme-loop.wav";
	const char* path_to_lose_effect = "resource/sound/333785__projectsu012__8-bit-failure-sound.wav";
	const char* path_to_eat_effect = "resource/sound/20265__koops__apple-crunch-02.wav";
};
