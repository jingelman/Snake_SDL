#pragma once
#include <SDL.h>
#include <string>
#include <SDL_image.h>


struct Texture
{
	SDL_Texture* mTexture = nullptr;	// 4 bits
	int16_t mWidth = 0;					// 16 bits
	int16_t mHeight = 0;				// 16 bits
};

inline bool loadFromFile(Texture &texture, SDL_Renderer* renderer, std::string path)
{
	//Get rid of preexisting texture
	//free();

	//The final texture
	SDL_Texture* newTexture = nullptr;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == nullptr)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == nullptr)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			texture.mWidth = loadedSurface->w;
			texture.mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	texture.mTexture = newTexture;
	return texture.mTexture != nullptr;
}

inline void Free()
{
	
}