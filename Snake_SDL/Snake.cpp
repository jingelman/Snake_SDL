#include "Snake.h"

#include "CoreManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "TimerManager.h"

#include "RandomGenerator.h"

#include <string>

#ifdef _DEBUG
#include <stdio.h>
#endif

constexpr int max(int a, int b) { return a > b ? a : b; }

#define FRAMERATE 60

enum class Direction
{
	UP,
	RIGHT,
	LEFT,
	DOWN
} direction;

enum RENDERQUALITY
{
	NEAREST,
	LINEAR,
	ANISOTROPIC
};

/*
* Match positions with sprites
*/
Snake::Snake()
{
	background_area = { 0, 0, C_SCREEN_WIDTH, C_SCREEN_HEIGHT };

	game_area = { 0, 0, C_GAMEAREA_WIDTH, C_GAMEAREA_HEIGHT };

	highscore_board_area = { C_GAMEAREA_WIDTH, 0, C_SCOREBOARDAREA_WIDTH, C_SCOREBOARDAREA_HEIGHT };

	text_board_area = { (C_GAMEAREA_WIDTH - C_TEXTBOARDAREA_WIDTH) / 2, (C_GAMEAREA_HEIGHT - C_TEXTBOARDAREA_HEIGHT) / 2, C_TEXTBOARDAREA_WIDTH, C_TEXTBOARDAREA_HEIGHT };

	head_sprites.push_back({ 3 * C_SPRITE_SIZE, 0 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	head_sprites.push_back({ 4 * C_SPRITE_SIZE, 0 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	head_sprites.push_back({ 3 * C_SPRITE_SIZE, 1 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	head_sprites.push_back({ 4 * C_SPRITE_SIZE, 1 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });

	tail_sprites.push_back({ 3 * C_SPRITE_SIZE, 2 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	tail_sprites.push_back({ 4 * C_SPRITE_SIZE, 2 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	tail_sprites.push_back({ 3 * C_SPRITE_SIZE, 3 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	tail_sprites.push_back({ 4 * C_SPRITE_SIZE, 3 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });

	body_sprites.push_back({ 0 * C_SPRITE_SIZE, 0 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	body_sprites.push_back({ 1 * C_SPRITE_SIZE, 0 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	body_sprites.push_back({ 2 * C_SPRITE_SIZE, 0 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	body_sprites.push_back({ 0 * C_SPRITE_SIZE, 1 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	body_sprites.push_back({ 2 * C_SPRITE_SIZE, 1 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });
	body_sprites.push_back({ 2 * C_SPRITE_SIZE, 2 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE });

	apple_sprite = { 0 * C_SPRITE_SIZE, 3 * C_SPRITE_SIZE, C_SPRITE_SIZE, C_SPRITE_SIZE };
}

/*
* Freedom
*/
Snake::~Snake()
{
	SoundManager::freeMusic();
	SoundManager::freeSoundEffect();
	TextureManager::freeTextures();
	CoreManager::freeCore();
}

/*
* Initiate the window, renderer and texture/sound
*/
bool Snake::init()
{
	bool success = true;

	TimerManager::startTimer();

	if (!CoreManager::initVideo || !CoreManager::initAudio)
		success = false;
	else
	{
		CoreManager::setRenderQuality(LINEAR);

		if (!CoreManager::CreateWindow("Snake WoW!", C_SCREEN_WIDTH, C_SCREEN_HEIGHT))
			success = false;
		else
		{
			if (!CoreManager::CreateRenderer())
				success = false;
			else
			{
				CoreManager::SetRenderColor(255, 255, 255, 255);

				if (!TextureManager::initTexture())
					success = false;

				if (!SoundManager::initSound())
					success = false;
			}
		}
	}

	return success;
}

/*
* Load the assets
*/
bool Snake::load_media()
{
	bool success = true;

	if (!SoundManager::loadMusic(path_to_music))
		success = false;

	if (!SoundManager::loadEffect(path_to_eat_effect))
		success = false;

	if (!SoundManager::loadEffect(path_to_lose_effect))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), path_to_background_texture))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), path_to_grass_texture))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), path_to_sprite_texture))
		success = false;

	if (!TextureManager::loadTexture(CoreManager::getRenderer(), path_to_board_texture))
		success = false;

	if (!TextureManager::loadFont(path_to_font, 80))
		success = false;

	return success;
}

/*
* Setup variables, timer, positions before a game is started
*/
void Snake::setup_game()
{
	SoundManager::pauseMusic();
	TimerManager::startTimer();

	rec_snake.clear();
	rec_snake.push_back({ 3 * C_TILE_SIZE, 3 * C_TILE_SIZE, C_TILE_SIZE, C_TILE_SIZE });
	rec_snake.push_back({ 2 * C_TILE_SIZE, 3 * C_TILE_SIZE, C_TILE_SIZE, C_TILE_SIZE });
	rec_snake.push_back({ 1 * C_TILE_SIZE, 3 * C_TILE_SIZE, C_TILE_SIZE, C_TILE_SIZE });

	apple_counter = 0;
	add_apple();

	velocity = C_START_SPEED;

	//m_hasLost = false;
	m_isPaused = true;
	render_intro_text = true;
	render_score_board = true;

	direction = Direction::RIGHT;
	update_sprites();
	render();

	render_intro_text = false;
}

/*
* Copy instruction text to the renderer
*/
void Snake::update_simulation(float dt)
{
	render();
	update_snake_position();
	update_sprites();
	update_apple_position();
}

/*
* Copy instruction text to the renderer
*/
void Snake::event_loop()
{
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		if (event.type == SDL_KEYDOWN)
		{
			m_isPaused = false;

			switch (event.key.keysym.sym)
			{
				case SDLK_UP: case SDLK_w:
					if (rec_snake[0].y <= rec_snake[1].y && !m_hasLost) {
						direction = Direction::UP;
					}
					break;
				case SDLK_RIGHT: case SDLK_d:
					if (rec_snake[0].x >= rec_snake[1].x && !m_hasLost) {
						direction = Direction::RIGHT;
					}
					break;
				case SDLK_LEFT: case SDLK_a:
					if (rec_snake[0].x <= rec_snake[1].x && !m_hasLost) {
						direction = Direction::LEFT;
					}
					break;
				case SDLK_DOWN: case SDLK_s:
					if (rec_snake[0].y >= rec_snake[1].y && !m_hasLost) {
						direction = Direction::DOWN;
					}
					break;
				case SDLK_LCTRL: case SDLK_RCTRL:
					if (!double_speed)
					{
						velocity *= C_SPEEDUP;
						double_speed = true;
					}
					else
					{
						velocity /= C_SPEEDUP;
						double_speed = false;
					}
					break;
				case SDLK_SPACE:
					if (m_hasLost)
					{
						m_hasLost = false;
						setup_game();
					}
					break;
				case SDLK_ESCAPE: quit = true;
				default:
					break;
			}
		}
	}
}

/*
* Copy instruction text to the renderer
*/
void Snake::game_loop()
{
	// Sound levels
	SoundManager::setMusicVolume(5);
	SoundManager::setEffectVolume(0, 30);
	SoundManager::setEffectVolume(1, 10);

	float distance = 0;
	float dt = 0;

	quit = false;
	render_score_board = true;

	setup_game();

	while (!quit)
	{
		event_loop();

		if (TimerManager::isTimerStarted() && !m_isPaused)
		{
			dt = TimerManager::getTicks() / 1000.f;

			if (!SoundManager::isMusicPlaying() || !SoundManager::isMusicPaused())
			{
				SoundManager::playMusic(0);
			}

			if (!hit_wall(rec_snake[0]) && !hit_object(rec_snake[0], true))
			{
				distance += velocity * dt;

				if (distance >= C_TILE_SIZE)
				{
					update_simulation(dt);
					distance = 0;
				}

				TimerManager::startTimer();
			}
			else
			{
				m_hasLost = true;

				render();

				TimerManager::stopTimer();
				SoundManager::pauseMusic();
				SoundManager::playEffect(1);

				#if _DEBUG
				printf("You ate %i apples!\n", apple_counter);
				#endif
			}

		}

		auto ticks = TimerManager::getTicks();
		if (ticks <= (1000.f / FRAMERATE))
		{
			SDL_Delay((1000.f / FRAMERATE) - ticks);
		}
	}
}

/*
* Updates the snakes position
*/
void Snake::update_snake_position()
{
	SDL_Rect temp;
	bool ateApple = false;

	switch (direction)
	{
		case Direction::UP:

			#if _DEBUG
			printf("Going up!\n");
			#endif

			temp = rec_snake[rec_snake.size() - 1];

			for (auto i = rec_snake.size() - 1; i > 0; --i)
			{
				rec_snake[i] = rec_snake[i - 1];
			}

			rec_snake[0].y -= C_TILE_SIZE;

			if (hit_apple(rec_snake[0]))
				ateApple = true;

			break;
		case Direction::RIGHT:

			#if _DEBUG
			printf("Going right!\n");
			#endif

			temp = rec_snake[rec_snake.size() - 1];

			for (auto i = rec_snake.size() - 1; i > 0; --i)
			{
				rec_snake[i] = rec_snake[i - 1];
			}

			rec_snake[0].x += C_TILE_SIZE;

			if (hit_apple(rec_snake[0]))
				ateApple = true;

			break;
		case Direction::LEFT:

			#if _DEBUG
			printf("Going left!\n");
			#endif

			temp = rec_snake[rec_snake.size() - 1];

			for (auto i = rec_snake.size() - 1; i > 0; --i)
			{
				rec_snake[i] = rec_snake[i - 1];
			}

			rec_snake[0].x -= C_TILE_SIZE;

			if (hit_apple(rec_snake[0]))
				ateApple = true;

			break;
		case Direction::DOWN:

			#if _DEBUG
			printf("Going down!\n");
			#endif

			temp = rec_snake[rec_snake.size() - 1];

			for (auto i = rec_snake.size() - 1; i > 0; --i)
			{
				rec_snake[i] = rec_snake[i - 1];
			}

			rec_snake[0].y += C_TILE_SIZE;

			if (hit_apple(rec_snake[0]))
				ateApple = true;

			break;
		default:
			break;
	}

	if (ateApple)
	{
		SoundManager::playEffect(0);

		rec_snake.push_back(temp);

		++apple_counter;

		add_apple();

		if (apple_counter % 5 == 0)
			velocity += C_TILE_SIZE;

		if (apple_counter > highscore_counter)
		{
			highscore_counter = apple_counter;
		}

		render_score_board = true;
	}
}

/*
* Updates the snakes sprites after position change
*/
void Snake::update_sprites()
{
	temp_position.clear();
	temp_position.resize(rec_snake.size());

	for (size_t i = 0; i < rec_snake.size(); ++i)
	{
		if (i == 0)
		{
			if (direction == Direction::UP)
				temp_position[i] = head_sprites[0];
			else if (direction == Direction::RIGHT)
				temp_position[i] = head_sprites[1];
			else if (direction == Direction::LEFT)
				temp_position[i] = head_sprites[2];
			else
				temp_position[i] = head_sprites[3];
		}
		else if (i == rec_snake.size() - 1)
		{
			if (rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y > rec_snake[i - 1].y) // up
				temp_position[i] = tail_sprites[0];
			else if (rec_snake[i].x < rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y) // right
				temp_position[i] = tail_sprites[1];
			else if (rec_snake[i].x > rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y) // left
				temp_position[i] = tail_sprites[2];
			else
				temp_position[i] = tail_sprites[3]; // down
		}
		else
		{
			if (rec_snake[i].x < rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y < rec_snake[i + 1].y ||
				rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y < rec_snake[i - 1].y && rec_snake[i].x < rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y)
				temp_position[i] = body_sprites[0];

			else if (rec_snake[i].x > rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x < rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y ||
				rec_snake[i].x < rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x > rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y)
				temp_position[i] = body_sprites[1];

			else if (rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y < rec_snake[i - 1].y && rec_snake[i].x > rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y ||
				rec_snake[i].x > rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y < rec_snake[i + 1].y)
				temp_position[i] = body_sprites[2];

			else if (rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y > rec_snake[i - 1].y && rec_snake[i].x < rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y ||
				rec_snake[i].x < rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y > rec_snake[i + 1].y)
				temp_position[i] = body_sprites[3];

			else if (rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y > rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y < rec_snake[i + 1].y ||
				rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y < rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y > rec_snake[i + 1].y)
				temp_position[i] = body_sprites[4];

			else if (rec_snake[i].x == rec_snake[i - 1].x && rec_snake[i].y > rec_snake[i - 1].y && rec_snake[i].x > rec_snake[i + 1].x && rec_snake[i].y == rec_snake[i + 1].y ||
				rec_snake[i].x > rec_snake[i - 1].x && rec_snake[i].y == rec_snake[i - 1].y && rec_snake[i].x == rec_snake[i + 1].x && rec_snake[i].y > rec_snake[i + 1].y)
				temp_position[i] = body_sprites[5];
		}
	}
}

/*
* Copy background texture to the renderer
*/
void Snake::render_pass_background()
{
	CoreManager::SetViewport(&background_area);
	CoreManager::RenderCopy(TextureManager::getTexture(0));
}

/*
* Copy gamearea texture to the renderer
*/
void Snake::render_pass_gamearea()
{
	CoreManager::SetViewport(&game_area);
	CoreManager::RenderCopy(TextureManager::getTexture(1));
}

/*
* Copy borad texture for highscore to the renderer
*/
void Snake::render_pass_highscore_board()
{
	CoreManager::SetViewport(&highscore_board_area);
	CoreManager::RenderCopy(TextureManager::getTexture(3));
}

/*
* Copy borad texture for text to the renderer
*/
void Snake::render_pass_text_board()
{
	CoreManager::SetViewport(&text_board_area);
	CoreManager::RenderCopy(TextureManager::getTexture(3));
}

/*
* Copy instruction text to the renderer
*/
void Snake::render_pass_instruction_text()
{
	CoreManager::SetViewport(&game_area);

	TextureManager::setText(CoreManager::getRenderer(), "Use the Arrow keys or WASD to navigate.", 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - 0.7*TextureManager::font.texture.mWidth) / 2;
	text_position.y = (C_GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight) / 3;
	text_position.w = 0.7*TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), "Press LCTRL/RCTRL to move faster.", 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - 0.7*TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.1*C_GAMEAREA_HEIGHT;
	text_position.w = 0.7*TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), "Press again to get normal speed.", 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - 0.7*TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.1*C_GAMEAREA_HEIGHT;
	text_position.w = 0.7*TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), "Press any key to start.", 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - 0.7*TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.1*C_GAMEAREA_HEIGHT;
	text_position.w = 0.7*TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);
}

/*
* Copy instruction text texture to the renderer
*/
void Snake::render_pass_start_text()
{
	CoreManager::SetViewport(&game_area);

	TextureManager::setText(CoreManager::getRenderer(), "Press any key to start", 255, 255, 255);
	text_position = { (C_GAMEAREA_WIDTH - TextureManager::font.texture.mWidth) / 2, (C_GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight) / 2, TextureManager::font.texture.mWidth, TextureManager::font.texture.mHeight };
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);
}

/*
* Copy instruction text texture to the renderer
*/
void Snake::render_pass_lost_text()
{
	CoreManager::SetViewport(&game_area);

	std::string buf = "You ate " + std::to_string(apple_counter) + " apples!";
	TextureManager::setText(CoreManager::getRenderer(), buf.c_str(), 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y = 0.4*(C_GAMEAREA_HEIGHT - TextureManager::font.texture.mHeight);
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), "Press spacebar to play again", 255, 255, 255);
	text_position.x = (C_GAMEAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.15*C_GAMEAREA_HEIGHT;
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);
}

/*
* Copy instruction text texture to the renderer
*/
void Snake::render_pass_highscore_text()
{
	CoreManager::SetViewport(&background_area);

	TextureManager::setText(CoreManager::getRenderer(), "High Score", 255, 255, 255);
	text_position.x = C_GAMEAREA_WIDTH + (C_SCOREBOARDAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y = 0.1*TextureManager::font.texture.mHeight;
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), std::to_string(highscore_counter).c_str(), 255, 255, 255);
	text_position.x = C_GAMEAREA_WIDTH + (C_SCOREBOARDAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.9*TextureManager::font.texture.mHeight;
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), "Your Score", 255, 255, 255);
	text_position.x = C_GAMEAREA_WIDTH + (C_SCOREBOARDAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.9*TextureManager::font.texture.mHeight;
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);

	TextureManager::setText(CoreManager::getRenderer(), std::to_string(apple_counter).c_str(), 255, 255, 255);
	text_position.x = C_GAMEAREA_WIDTH + (C_SCOREBOARDAREA_WIDTH - TextureManager::font.texture.mWidth) / 2;
	text_position.y += 0.9*TextureManager::font.texture.mHeight;
	text_position.w = TextureManager::font.texture.mWidth;
	text_position.h = TextureManager::font.texture.mHeight;
	CoreManager::RenderCopy(TextureManager::getFontTexture(), nullptr, &text_position);
}

/*
* Copy instruction text texture to the renderer
*/
void Snake::render_pass_snake()
{
	for (size_t i = 0; i < temp_position.size(); ++i)
	{
		CoreManager::RenderCopy(TextureManager::getTexture(2), &temp_position[i], &rec_snake[i]);
	}

	CoreManager::RenderCopy(TextureManager::getTexture(2), &apple_sprite, &apple_position);
}

/*
* Render the window
*/
void Snake::render()
{
	if (first_game)
	{
		render_pass_background();
		render_pass_gamearea();
		render_pass_text_board();
		render_pass_instruction_text();
		render_pass_highscore_board();
		render_pass_highscore_text();
		first_game = false;
	}
	else {
		if (render_score_board)
		{
			render_pass_background();
			render_pass_highscore_board();
			render_pass_highscore_text();
			render_score_board = false;
		}

		render_pass_gamearea();

		if (!m_hasLost)
			render_pass_snake();

		if (render_intro_text)
		{
			render_pass_text_board();
			render_pass_start_text();
		}

		if (m_hasLost)
		{
			render_pass_snake();
			render_pass_text_board();
			render_pass_lost_text();
		}

	}
	CoreManager::render();
}

/*
* Calculate a pseudo random position to render the apple at
* The position is within the game area and will not be on the snake
*/
void Snake::add_apple()
{
	bool freePos;

	SDL_Rect temp;

	do
	{
		freePos = true;

		auto posX = randomNumber(0, C_TILE_WIDTH - 1);
		auto posY = randomNumber(0, C_TILE_HEIGHT - 1);

		temp = { posX * C_TILE_SIZE , posY * C_TILE_SIZE , C_TILE_SIZE, C_TILE_SIZE };

		for (auto p : rec_snake)
		{
			if (p.x == temp.x && p.y == temp.y)
				freePos = false;
		}
	} while (!freePos);

	apple_position = temp;
}

/*
* A pseudo random function to the move apple around
* The chance gets higher with the highscore
*/
void Snake::update_apple_position()
{
	auto max_val = max(1, 50 - pow((0.2*apple_counter), 2));

	auto move = randomNumber(0, max_val);

	if (move == 0)
	{
		SDL_Rect temp;
		auto move_direction = randomNumber(0, 1);

		auto x_0 = rec_snake[0].x;
		auto x_1 = C_GAMEAREA_WIDTH - rec_snake[0].x;
		auto y_0 = rec_snake[0].y;
		auto y_1 = C_GAMEAREA_HEIGHT - rec_snake[0].y;

		if (move_direction)
		{
			if (x_0 > x_1 && apple_position.x != 0)
			{
				temp = apple_position;
				temp.x -= C_TILE_SIZE;

				if (!hit_object(temp, false))
					apple_position.x -= C_TILE_SIZE;
			}
			else if (apple_position.x + C_TILE_SIZE != C_GAMEAREA_WIDTH)
			{
				temp = apple_position;
				temp.x += C_TILE_SIZE;

				if (!hit_object(temp, false))
					apple_position.x += C_TILE_SIZE;
			}
		}
		else
		{
			if (y_0 < y_1 && (apple_position.y + C_TILE_SIZE) != C_GAMEAREA_HEIGHT)
			{
				temp = apple_position;
				temp.y += C_TILE_SIZE;

				if (!hit_object(temp, false))
					apple_position.y += C_TILE_SIZE;
			}
			else if (apple_position.y != 0)
			{
				temp = apple_position;
				temp.y -= C_TILE_SIZE;

				if (!hit_object(temp, false))
					apple_position.y -= C_TILE_SIZE;
			}
		}
	}
}

/*
* Checks if the snake have collided with a apple
*/
bool Snake::hit_apple(const SDL_Rect& head) const
{
	return head.x == apple_position.x && head.y == apple_position.y;
}

/*
* Checks if the snake have collided with the wall
*/
bool Snake::hit_wall(const SDL_Rect& head) const
{
	bool hit = false;

	if (head.x < 0 || head.y < 0 || head.x + head.w > C_GAMEAREA_WIDTH || head.y + head.h > C_GAMEAREA_HEIGHT)
	{
		#if _DEBUG
		printf("You hit the wall! \n");
		#endif

		hit = true;
	}

	return hit;
}

/*
* Checks if the snake have collided with selected object
*/
bool Snake::hit_object(const SDL_Rect &object, bool head)
{
	bool hit = false;

	size_t p;

	if (head)
		p = 1;
	else
		p = 0;

	for (0; p < rec_snake.size(); ++p)
	{
		auto& snakePart = rec_snake[p];

		if (snakePart.x == object.x && snakePart.y == object.y)
		{
			#if _DEBUG
			if (head)
				printf("You hit your own body! \n");
			else
				printf("Apple about to collid, didnt move! \n");
			#endif

			hit = true;
		}
	}

	return hit;
}
