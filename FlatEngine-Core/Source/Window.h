#pragma once
#include <SDL.h>
#include <string>


namespace Window
{
	bool Init(char* title, int width, int height);
	void Close();
	void SetScreenDimensions(int width, int height);
	SDL_Renderer* GetRenderer();
	void Render();
	void SetFullscreen(bool _isFullscreen);

	//Variables
	extern SDL_Window* window;
	extern SDL_Renderer* renderer;
	extern SDL_Surface* screenSurface;

	//Window dimensions
	extern int WINDOW_WIDTH;
	extern int WINDOW_HEIGHT;
	extern bool _isFullscreen;
};

