#pragma once

#include <cstdio>
#include <SDL_mixer.h>

struct Music
{
	Mix_Music* sound = nullptr;
};

inline bool initMusic()
{
	bool success = true;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

inline bool loadMusic(Music& s, const char* path)
{
	bool success = true;

	s.sound = Mix_LoadMUS(path);
	if (s.sound == nullptr)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

inline void playMusic(Music& s)
{
	Mix_PlayMusic(s.sound, -1);
}

inline bool isMusicPlaying()
{
	return Mix_PlayingMusic() != 0;
}

inline bool isMusicPaused()
{
	return Mix_PausedMusic() != 1;
}

inline void freeMusic(Music& s)
{
	Mix_FreeMusic(s.sound);
}
