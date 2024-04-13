#include "Sprite.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Sprite::Sprite(long myID, long parentID)
	{
		this->SetType(Component::ComponentTypes::Sprite);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->texture = nullptr;
		this->textureWidth = 0;
		this->textureHeight = 0;
		this->offset = { 0,0 };
		this->path = "";
		this->renderOrder = 0;
	}

	Sprite::Sprite(std::shared_ptr<Sprite> toCopy, long newParentID)
	{
		this->SetType(Component::ComponentTypes::Sprite);
		this->SetID(GetNextComponentID());
		this->SetParentID(newParentID);
		this->texture = toCopy->GetTexture();
		this->textureWidth = toCopy->GetTextureWidth();
		this->textureHeight = toCopy->GetTextureHeight();
		this->offset = toCopy->GetOffset();
		this->path = toCopy->GetPath();
		this->renderOrder = toCopy->GetRenderOrder();
	}


	Sprite::~Sprite()
	{
	}


	std::string Sprite::GetData()
	{
		json jsonData = { 
			{ "type", "Sprite" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "texture", this->path },
			{ "texture_width", this->textureWidth },
			{ "texture_height", this->textureHeight },
			{ "xOffset", this->offset.x },
			{ "yOffset", this->offset.y },
			{ "renderOrder", this->renderOrder }
,		};
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
				this->textureWidth = (float)texture->getWidth();
				this->textureHeight = (float)texture->getHeight();

				// Set pivot point to the center of the texture by default
				this->offset = { this->textureWidth / 2, this->textureHeight / 2 };
			}
		}
		else
			this->texture = nullptr;
	}


	void Sprite::SetOffset(Vector2 offset)
	{
		this->offset = offset;
	}


	Vector2 Sprite::GetOffset()
	{
		return this->offset;
	}


	SDL_Texture* Sprite::GetTexture()
	{
		return this->texture;
	}


	float Sprite::GetTextureWidth()
	{
		return (float)this->textureWidth;
	}


	float Sprite::GetTextureHeight()
	{
		return (float)this->textureHeight;
	}

	
	std::string Sprite::GetPath()
	{
		return this->path;
	}

	void Sprite::SetRenderOrder(int order)
	{
		this->renderOrder = order;
	}

	int Sprite::GetRenderOrder()
	{
		return this->renderOrder;
	}

	void Sprite::RemoveTexture()
	{
		this->texture = NULL;
	}
}
