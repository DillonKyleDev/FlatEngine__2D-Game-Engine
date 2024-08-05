#include "Window.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"



namespace Window
{
	SDL_Window* W_Window = nullptr;
	SDL_Renderer* W_Renderer = nullptr;
	SDL_Surface* W_ScreenSurface = nullptr;

	//Window dimensions
	std::string W_title = "";
	int W_windowWidth = 900;
	int W_windowHeight = 600;
	bool W_isFullscreen = false;


	void SetFullscreen(bool _isFullscreen)
	{
		W_isFullscreen = _isFullscreen;

		SDL_SetWindowFullscreen(W_Window, _isFullscreen);
		//SDL_ShowCursor(true);
	}

	void SetTitle(std::string title)
	{
		W_title = title;
		RecreateWindow();
	}

	bool CreateWindow(std::string title, int width, int height)
	{
		W_title = title;
		W_windowWidth = width;
		W_windowHeight = height;
		return RecreateWindow();
	}

	bool RecreateWindow()
	{
		bool b_success = true;

		if (W_Renderer != NULL)
			SDL_DestroyRenderer(W_Renderer);
		if (W_Window != NULL)
			SDL_DestroyWindow(W_Window);

		// Create window with SDL_Renderer graphics context
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		W_Window = SDL_CreateWindow("FlatEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_windowWidth, W_windowHeight, window_flags);
		// Vsync on
		//W_Renderer = SDL_CreateRenderer(W_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		// Vsync off
		W_Renderer = SDL_CreateRenderer(W_Window, -1, 0);
		SDL_RenderSetLogicalSize(W_Renderer, W_windowWidth, W_windowHeight);
		SDL_RenderSetIntegerScale(W_Renderer, SDL_bool(true));
		if (W_Window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			b_success = false;
		}
		else
		{
			//Create vsynced renderer for window
			//W_Renderer = SDL_CreateRenderer(W_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (W_Renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				b_success = false;
			}
		}

		if (b_success)
		{
			//Get window surface down here so we don't have to delete it if the SDL_image init fails anyway
			W_ScreenSurface = SDL_GetWindowSurface(W_Window);
		}

		return b_success;
	}


	bool Init(std::string title, int width, int height)
	{
		return CreateWindow(title, width, height);
	}


	void Close()
	{
		//Destroy window
		SDL_DestroyRenderer(W_Renderer);
		SDL_DestroyWindow(W_Window);
		W_Renderer = NULL;
		W_Window = NULL;
	}


	bool SetScreenDimensions(int width, int height)
	{
		W_windowWidth = width;
		W_windowHeight = height;
		return RecreateWindow();
	}


	SDL_Renderer* Window::GetRenderer()
	{
		return W_Renderer;
	}


	void Render()
	{
		//Clear the screen
		SDL_SetRenderDrawColor(W_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(W_Renderer);

		//Update the screen
		SDL_RenderPresent(W_Renderer);
	}
}