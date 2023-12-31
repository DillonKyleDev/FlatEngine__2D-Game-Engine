#include "Sprite.h"


namespace FlatEngine
{
	Sprite::Sprite()
	{
		this->texture = NULL;
		this->SetType(Component::ComponentTypes::Sprite);
		this->textureWidth = 0;
		this->textureHeight = 0;
		this->SetTexture("assets/images/Dot.png");
	}


	Sprite::~Sprite()
	{

	}


	std::string Sprite::GetData()
	{
		json jsonData = { 
			{ "type", "Sprite" },
			{ "texture", this->path },
			{ "texture_width", this->textureWidth },
			{ "texture_height", this->textureHeight }
		};
		std::string data = jsonData.dump();
		return data;
	}


	void Sprite::SetTexture(std::string path)
	{
		if (path != "")
		{
			// Save path for referencing later if needed
			this->path = path;
			Texture* texture = new Texture();
			if (texture->loadFromFile(path))
			{
				this->texture = texture->getTexture();
				this->textureWidth = texture->getWidth();
				this->textureHeight = texture->getHeight();
			}
		}
	}


	SDL_Texture* Sprite::GetTexture()
	{
		return this->texture;
	}


	float Sprite::GetTextureWidth()
	{
		return this->textureWidth;
	}


	float Sprite::GetTextureHeight()
	{
		return this->textureHeight;
	}

	
	std::string Sprite::GetPath()
	{
		return this->path;
	}


	void Sprite::RemoveTexture()
	{
		this->texture = NULL;
	}
}
