#pragma once
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Texture.h"
#include "Button.h"

namespace TextureManager
{
	bool LoadTextures();
	void RenderTextures();
	void Cleanup();

	//Variables
	extern std::vector<Texture> loadedTextures;

	//Set text color as black
	extern SDL_Color textColor;
	//In memory text stream
	extern std::stringstream timeText;

	//Font
	extern TTF_Font* font;
	//Texture that we'll generate to from the font
	extern Texture fontTexture;

	//dot Texture
	extern Texture dot;
	extern Texture dots;
	extern Texture button;

	//Buttons
	//extern Button goldButton;
};

