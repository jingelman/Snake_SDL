#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace TextureManager
{
	struct Texture {
		SDL_Texture* mTexture = nullptr; // 4 bits
		int16_t mWidth = 0; // 16 bits
		int16_t mHeight = 0; // 16 bits
	};
	std::vector<Texture> textures;

	struct Font {
		TTF_Font *mFont = nullptr;
		Texture texture;
	} font;

	static bool initTexture()
	{
		bool success = true;

		uint8_t imgFlags = IMG_INIT_PNG;
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

		if (TTF_Init() == -1)
		{
			printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
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

	static bool loadFont(const char* path, uint8_t size)
	{
		//Loading success flag
		bool success = true;

		if (font.mFont != nullptr)
		{
			TTF_CloseFont(font.mFont);
			font.mFont = nullptr;
		}

		//Open the font
		font.mFont = TTF_OpenFont(path, size);
		if (font.mFont == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}

		return success;
	}


	bool setText(SDL_Renderer* ren, const char* textureText, uint8_t r, uint8_t g, uint8_t b)
	{

		if (font.texture.mTexture != nullptr)
		{
			SDL_DestroyTexture(font.texture.mTexture);
			font.texture.mTexture = nullptr;
		}

		bool success = true;

		SDL_Color color = { r, g, b };

		//Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(font.mFont, textureText, color);
		if (textSurface == nullptr)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
			return success;
		}

		Texture newTexture;

		//Create texture from surface pixels
		newTexture.mTexture = SDL_CreateTextureFromSurface(ren, textSurface);
		if (newTexture.mTexture == nullptr)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			newTexture.mWidth = textSurface->w;
			newTexture.mHeight = textSurface->h;

			//Get rid of old loaded surface
			SDL_FreeSurface(textSurface);

			font.texture = newTexture;
		}

		return success;
	}

	static Texture& getTexture(uint8_t ind)
	{
		return textures[ind];
	}

	static Texture& getFontTexture()
	{
		return font.texture;
	}

	static void freeTextures()
	{
		for (auto &tex : textures)
		{
			SDL_DestroyTexture(tex.mTexture);
			tex.mTexture = nullptr;
		}
		IMG_Quit();

		if (font.mFont != nullptr)
		{
			TTF_CloseFont(font.mFont);
			font.mFont = nullptr;
		}
		TTF_Quit();
	}
};