#pragma once
#include "Component.h"
#include "Texture.h"
#include <SDL.h>

class Sprite : public Component
{
public:
	Sprite();
	~Sprite();
	void SetTexture(SDL_Texture* texture);
	void RemoveTexture();

private:
	SDL_Texture* texture;
};

