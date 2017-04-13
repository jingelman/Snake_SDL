#pragma once

#include <SDL.h>

#include "TextureManager.h"

namespace CoreManager {

	struct Core
	{
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
	} core;

	static bool initVideo()
	{
		bool success = true;
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}

		return success;
	}

	static bool initAudio()
	{
		bool success = true;
		if (SDL_Init(SDL_INIT_AUDIO) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}

		return success;
	}

	static bool setRenderQuality(short q)
	{
		bool success = true;

		if (q > 2)
		{
			printf("The number must be between 0 and 2");
			success = false;
			return success;
		}

		const char aChar = '0' + q;

		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, &aChar))
		{
			printf("Warning: Linear texture filtering not enabled!");
			success = false;
			return success;
		}

		return success;
	}

	static bool CreateWindow(const char* title, const Uint32 w, const Uint32 h)
	{
		bool success = true;

		if (core.window != nullptr)
		{
			success = false;
		}
		else {

			core.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
			if (core.window == nullptr)
			{
				printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
				success = false;
			}
		}

		return success;
	}

	static bool CreateRenderer()
	{
		bool success = true;

		if (core.window == nullptr)
		{
			success = false;
		}
		else {

			core.renderer = SDL_CreateRenderer(core.window, -1, SDL_RENDERER_ACCELERATED);
			if (core.renderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}

			return success;
		}
	}

	static bool SetRenderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		bool success = true;

		if (core.window == nullptr || core.renderer == nullptr)
		{
			printf("Window and/or Renderer is not declared");
			success = false;
		}
		else
		{
			if (r > 255)
				r = 255;

			if (g > 255)
				g = 255;

			if (b > 255)
				b = 255;

			if (a > 255)
				a = 255;

			SDL_SetRenderDrawColor(core.renderer, r, g, b, a);
		}


		return success;
	}


	SDL_Renderer* getRenderer()
	{
		return core.renderer;
	}

	static void clearRenderer()
	{
		if (core.window == nullptr || core.renderer == nullptr)
		{
			printf("Window and/or Renderer is not declared");
			return;
		}

		SDL_RenderClear(core.renderer);
	}

	static void SetViewport(const SDL_Rect *rec)
	{
		SDL_RenderSetViewport(core.renderer, rec);
	}

	static void RenderCopy(TextureManager::Texture &t, const SDL_Rect *srec = nullptr, const SDL_Rect *drec = nullptr)
	{
		SDL_RenderCopy(core.renderer, t.mTexture, srec, drec);
	}

	static void render()
	{
		SDL_RenderPresent(core.renderer);
	}

	static void freeCore()
	{
		if (core.renderer != nullptr)
		{
			SDL_DestroyRenderer(core.renderer);
			core.renderer = nullptr;
		}

		if (core.window != nullptr)
		{
			SDL_DestroyWindow(core.window);
			core.window = nullptr;
		}

		SDL_Quit();
	}

};