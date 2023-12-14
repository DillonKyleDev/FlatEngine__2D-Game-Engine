#include "Button.h"


Button::Button()
{
	this->position = { 0, 0 };
}

Button::~Button()
{
	
}

void Button::setSpriteTexture(std::string path)
{
	this->sprite.loadFromFile(path);
}

//Set top left position
void Button::setPosition(int x, int y)
{
	this->position = { x, y };
}

//Handle events
void Button::handleEvent(SDL_Event* e)
{
	//e->type == SDL_MOUSEMOTION
	if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);
	}
}

//Render buttton sprite
void Button::render()
{
	this->sprite.render(Window::WINDOW_WIDTH / 2, Window::WINDOW_HEIGHT / 2);
}