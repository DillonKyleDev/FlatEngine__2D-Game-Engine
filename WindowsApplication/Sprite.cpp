#include "Sprite.h"


namespace FlatEngine
{
	Sprite::Sprite()
	{
		this->texture = NULL;
		this->SetType(Component::ComponentTypes::Sprite);
	}


	Sprite::~Sprite()
	{

	}


	std::string Sprite::GetData()
	{
		json jsonData = { 
			{ "type", "Sprite" },
			{ "texture", this->path }
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

			Texture texture;
			if (texture.loadFromFile(path))
			{
				this->texture = texture.getTexture();
			}
		}
	}


	SDL_Texture* Sprite::GetTexture()
	{
		return this->texture;
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
