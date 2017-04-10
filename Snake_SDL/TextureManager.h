#pragma once

#include <SDL.h>
#include <SDL_image.h>

namespace TextureManager
{
	struct Texture {
		SDL_Texture* mTexture = nullptr; // 4 bits
		int16_t mWidth = 0; // 16 bits
		int16_t mHeight = 0; // 16 bits
	};
	std::vector<Texture> textures;

	static void freeTexture()
	{
		for(auto &tex : textures)
		{
			SDL_DestroyTexture(tex.mTexture);
			tex.mTexture = nullptr;
		}
	}

	static bool initTexture()
	{
		bool success = true;

		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			success = false;
		}

		imgFlags = IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			success = false;
		}

		return success;
	}

	static bool loadTexture(SDL_Renderer* ren, const char* path)
	{
		bool success = true;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path);
		if (loadedSurface == nullptr)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
			success = false;
			return success;
		}

		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		Texture newTexture;

		//Create texture from surface pixels
		newTexture.mTexture = SDL_CreateTextureFromSurface(ren, loadedSurface);
		if (newTexture.mTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
			success = false;
		}
		else
		{
			//Get image dimensions
			newTexture.mWidth = loadedSurface->w;
			newTexture.mHeight = loadedSurface->h;

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);

			//t.mTexture = newTexture.mTexture;
			textures.push_back(newTexture);
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

};