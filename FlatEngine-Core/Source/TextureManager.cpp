#include "TextureManager.h"
#include "Texture.h"
#include "Window.h"

namespace FlatEngine
{
	TextureManager::TextureManager()
	{
		//Set text color as black
		SDL_Color textColor = { 0, 0, 0, 225 };
		TTF_Font* font = NULL;
		loadedTextures = std::vector<Texture>();
		fontTexture = Texture();
	}

	bool TextureManager::LoadTextures()
	{
		//Loading success flag
		bool success = true;

		font = TTF_OpenFont("C:/Users/Dillon Kyle/source/repos/FlatEngine3D/FlatEngine-Core/Source/assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
		if (font == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}

		//Render the text image
		SDL_Color textColor = { 0, 0, 0 };

		return success;
	}


	void TextureManager::RenderTextures()
	{
		SDL_Renderer* renderer = Window::GetRenderer();
		int winWidth = Window::WINDOW_WIDTH;
		int winHeight = Window::WINDOW_HEIGHT;
		int centerX = winWidth / 2;
		int centerY = winHeight / 2;

		//Clear renderer
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		//Update the screen
		//SDL_RenderPresent(renderer);
		//fontTexture.render((winWidth - fontTexture.getWidth()) / 2, (winHeight - fontTexture.getHeight()) / 2);
	}

	void TextureManager::Cleanup()
	{
		//Free global font
		TTF_CloseFont(font);
		font = NULL;
	}
}