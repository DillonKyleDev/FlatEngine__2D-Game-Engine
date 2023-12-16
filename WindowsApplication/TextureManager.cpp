#include "TextureManager.h"

namespace TextureManager
{
	//Set text color as black
	SDL_Color textColor = { 0, 0, 0, 225 };
	//In memory text stream
	std::stringstream timeText;

	//Font
	TTF_Font* font;
	//Texture that we'll generate to from the font
	Texture fontTexture;

	//dot Texture
	Texture dot;
	Texture dots;
	Texture button;

	bool LoadTextures()
	{
		font = TTF_OpenFont("C:/Users/Dillon Kyle/fonts/Cinzel/Cinzel-Black.ttf", 46);
		if (font == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		}

		//Loading success flag
		bool success = true;

		//Load png
		dot.loadFromFile("assets/images/Dot.png");
		dots.loadFromFile("assets/images/Dots.png");
		button.loadFromFile("assets/images/SingleButton.png");
		button.setDimensions(button.getWidth() * 3, button.getHeight() * 3);

		//Render the text image
		SDL_Color textColor = { 0, 0, 0 };
		if (!fontTexture.loadFromRenderedText("Call Ally", textColor, font))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}

		return success;
	}


	void RenderTextures()
	{
		SDL_Renderer* renderer = Window::GetRenderer();
		int winWidth = Window::WINDOW_WIDTH;
		int winHeight = Window::WINDOW_HEIGHT;
		int centerX = winWidth / 2;
		int centerY = winHeight / 2;

		//Clear renderer
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		//Apply the image
		//dot->blitSurface(Window::screenSurface);
		//Update the surface
		//SDL_UpdateWindowSurface(Window::window);

		//Render pngs
		dot.render(centerX, centerY);
		dots.render(centerX, centerY);

		//Render current frame

		//Window::Render();

		//Update the screen
		SDL_RenderPresent(renderer);
		fontTexture.render((winWidth - fontTexture.getWidth()) / 2, (winHeight - fontTexture.getHeight()) / 2);
	}

	void Cleanup()
	{
		//Free global font
		TTF_CloseFont(font);
		font = NULL;
	}

}