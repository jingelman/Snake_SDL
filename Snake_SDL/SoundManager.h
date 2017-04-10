﻿#pragma once

#include <cstdio>
#include <SDL_mixer.h>

namespace SoundManager
{
	struct Music
	{
		Mix_Music* sound = nullptr;
	};

	std::vector<Music> music;

	struct Effect
	{
		Mix_Chunk* effect = nullptr;
	};

	std::vector<Effect> effects;

	static bool initSound()
	{
		bool success = true;

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}

		return success;
	}

	static bool loadMusic(const char* path)
	{
		bool success = true;

		Music m;

		m.sound = Mix_LoadMUS(path);
		if (m.sound == nullptr)
		{
			printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}

		music.push_back(m);

		return success;
	}

	static bool loadEffect(const char* path)
	{
		bool success = true;

		Effect e;

		e.effect = Mix_LoadWAV(path);
		if (e.effect == nullptr)
		{
			printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}

		effects.push_back(e);

		return success;
	}

	static void playMusic(short ind)
	{
		Mix_PlayMusic(music[ind].sound, -1);
	}

	static bool isMusicPlaying()
	{
		return Mix_PlayingMusic() != 0;
	}

	static bool isMusicPaused()
	{
		return Mix_PausedMusic() != 1;
	}

	static void pauseMusic()
	{
		Mix_PauseMusic();
	}

	static void resumeMusic()
	{
		Mix_ResumeMusic();
	}

	static void freeMusic()
	{
		for (auto& m : music)
		{
			Mix_FreeMusic(m.sound);
		}
	}

	static void playEffect(short ind)
	{
		Mix_PlayChannel(-1, effects[ind].effect , 0);
	}

	static void freeSoundEffect()
	{
		for (auto& e : effects)
		{
			Mix_FreeChunk(e.effect);
		}
	}
};
