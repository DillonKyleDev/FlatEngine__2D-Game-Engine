#pragma once
#include <SDL.h>
#include <string>


namespace Window
{
	bool CreateWindow(std::string title, int width, int height);
	bool RecreateWindow();
	bool Init(std::string title, int width, int height);
	void Close();
	bool SetScreenDimensions(int width, int height);
	void SetTitle(std::string title);
	SDL_Renderer* GetRenderer();
	void Render();
	void SetFullscreen(bool b_isFullscreen);

	//Variables
	extern SDL_Window* W_Window;
	extern SDL_Renderer* W_Renderer;
	extern SDL_Surface* W_ScreenSurface;

	//Window dimensions
	extern std::string W_title;
	extern int W_windowWidth;
	extern int W_windowHeight;
	extern bool W_isFullscreen;
};

