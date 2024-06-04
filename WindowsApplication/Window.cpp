#include "Window.h"




namespace Window
{
	SDL_Window* Window::window;
	SDL_Renderer* Window::renderer;
	SDL_Surface* Window::screenSurface;

	//Window dimensions
	int Window::WINDOW_WIDTH;
	int Window::WINDOW_HEIGHT;
	bool _isFullscreen = false;


	void SetFullscreen(bool _isFullscreen)
	{
		Window::_isFullscreen = _isFullscreen;

		SDL_SetWindowFullscreen(window, _isFullscreen);
		//SDL_ShowCursor(true);
	}


	bool Init(char* title, int width, int height)
	{
		bool success = true;

		Window::SetScreenDimensions(width, height);

		//Create window
		//Window::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Window::WINDOW_WIDTH, Window::WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

		// Create window with SDL_Renderer graphics context
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		Window::window = SDL_CreateWindow("FlatEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
		// Vsync on
		//Window::renderer = SDL_CreateRenderer(Window::window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		// Vsync off
		Window::renderer = SDL_CreateRenderer(Window::window, -1, 0);

		if (Window::window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			//Window::renderer = SDL_CreateRenderer(Window::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (Window::renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
		}

		if (success)
		{
			//Get window surface down here so we don't have to delete it if the SDL_image init fails anyway
			Window::screenSurface = SDL_GetWindowSurface(Window::window);
		}

		return success;
	}


	void Close()
	{
		//Destroy window
		SDL_DestroyRenderer(Window::renderer);
		SDL_DestroyWindow(Window::window);
		Window::renderer = NULL;
		Window::window = NULL;
	}


	void SetScreenDimensions(int width, int height)
	{
		Window::WINDOW_WIDTH = width;
		Window::WINDOW_HEIGHT = height;
	}


	SDL_Renderer* Window::GetRenderer()
	{
		return Window::renderer;
	}


	void Render()
	{
		//Clear the screen
		SDL_SetRenderDrawColor(Window::renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(Window::renderer);

		//Update the screen
		SDL_RenderPresent(Window::renderer);
	}
}