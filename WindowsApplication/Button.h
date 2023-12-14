#pragma once
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Texture.h"

class Button
{
public:
	Button();

	~Button();

	void setSpriteTexture(std::string path);

	//Set top left position
	void setPosition(int x, int y);

	//Handle events
	void handleEvent(SDL_Event* e);

	//Render buttton sprite
	void render();

private:
	//Top left position of button
	SDL_Point position;
	//Loaded image texture for button
	Texture sprite;
};

