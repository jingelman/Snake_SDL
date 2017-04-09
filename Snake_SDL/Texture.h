#pragma once
#include <SDL.h>
#include <string>
#include <SDL_image.h>

// Helper function
inline IMG_InitFlags getImageType(const char* path)
{
	int index = -1;
	for (int i = strlen(path) - 1; i >= 0; --i)
	{
		if (path[i] == '.')
		{
			index = i;
			break;
		}
	}

	if (path + index == "png")
		return IMG_INIT_PNG;
	if (path + index == "jpg")
		return IMG_INIT_JPG;
}


struct Texture
{
	SDL_Texture* mTexture = nullptr; // 4 bits
	int16_t mWidth = 0; // 16 bits
	int16_t mHeight = 0; // 16 bits
};

inline void freeTexture(Texture& t)
{
	SDL_DestroyTexture(t.mTexture);
	t.mTexture = nullptr;
}

inline bool initTexture(const char* path)
{
	bool success = true;

	int imgFlags = getImageType(path);
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		success = false;
	}

	return success;
}

inline bool loadTexture(Texture& t, SDL_Renderer* ren, const char* path)
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

	freeTexture(t);
	Texture newTexture;

	//Color key image
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

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
		t.mWidth = loadedSurface->w;
		t.mHeight = loadedSurface->h;

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);

		t.mTexture = newTexture.mTexture;
	}

	return success;
}

inline bool setRenderQuality(short q)
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
