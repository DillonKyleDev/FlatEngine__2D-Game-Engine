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
		Texture(const Texture* toCopy);
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
		SDL_Texture* m_texture;
		//Loaded from file surface
		SDL_Surface* m_surface;
		TTF_Font* m_font;
		std::string m_path;

		//Image dimensions
		int m_textureWidth;
		int m_textureHeight;
	};
}