#pragma once

#include <SDL_mixer.h>

#include <cstdio>

struct SoundEffect
{
	Mix_Chunk* effect = nullptr;
};

inline bool initSound()
{
	bool success = true;

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}


inline bool loadEffect(SoundEffect& e, const char* path)
{
	bool success = true;

	e.effect = Mix_LoadWAV(path);
	if (e.effect == nullptr)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

inline void playEffect(SoundEffect e)
{
	Mix_PlayChannel(-1, e.effect, 0);
}

inline void freeSoundEffect(SoundEffect& s)
{
	Mix_FreeChunk(s.effect);
}
