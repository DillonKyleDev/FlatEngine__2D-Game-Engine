#include "Sprite.h"

Sprite::Sprite()
{
	this->texture = NULL;
	this->SetType(Component::ComponentTypes::Sprite);
}


Sprite::~Sprite()
{

}


void Sprite::SetTexture(SDL_Texture* texture)
{
	this->texture = texture;
}


void Sprite::RemoveTexture()
{
	this->texture = NULL;
}