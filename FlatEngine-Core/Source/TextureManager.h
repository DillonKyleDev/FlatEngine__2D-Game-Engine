#pragma once
#include "FlatEngine.h"

#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>


namespace FlatEngine
{
	class Texture;

	class TextureManager {
	public:
		TextureManager();
		bool LoadTextures();
		void RenderTextures();
		void Cleanup();

	private:
		std::vector<Texture> loadedTextures;
		SDL_Color textColor;
		TTF_Font* font;
		Texture fontTexture;
	};
};

