#pragma once
#include <SDL.h>
#include <string>
#include <sstream>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Window.h"


namespace FlatEngine
{
	class Texture
	{
	public:
		Texture(std::string path = "");
		~Texture();
		bool LoadFromFile(std::string path);
		bool LoadSurface(std::string path, SDL_Surface* screenSurface);
		//Creates image from font string
		bool LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font);
		//Deallocates texture
		void FreeTexture();
		//Returns SDL_Texture
		SDL_Texture* GetTexture();
		//Deallocates surface
		void FreeSurface();
		//Get image dimensions
		int GetWidth();
		int GetHeight();
		//Set image dimensions
		void SetDimensions(int width, int height);
		void SetFont(std::string path);

	private:
		//The actual hardware texture
		SDL_Texture* texture;
		//Loaded from file surface
		SDL_Surface* surface;
		TTF_Font* font;

		//Image dimensions
		int textureWidth;
		int textureHeight;
	};
}