#include "Sprite.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Sprite::Sprite(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::Sprite);
		SetID(myID);
		SetParentID(parentID);
		texture = nullptr;
		textureWidth = 0;
		textureHeight = 0;
		offset = { 0,0 };
		path = "";
		renderOrder = 0;
	}

	Sprite::Sprite(std::shared_ptr<Sprite> toCopy, long newParentID)
	{
		SetType(Component::ComponentTypes::Sprite);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		texture = toCopy->GetTexture();
		textureWidth = toCopy->GetTextureWidth();
		textureHeight = toCopy->GetTextureHeight();
		offset = toCopy->GetOffset();
		path = toCopy->GetPath();
		renderOrder = toCopy->GetRenderOrder();
	}

	Sprite::~Sprite()
	{
	}

	std::string Sprite::GetData()
	{
		json jsonData = { 
			{ "type", "Sprite" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "texture", path },
			{ "texture_width", textureWidth },
			{ "texture_height", textureHeight },
			{ "xOffset", offset.x },
			{ "yOffset", offset.y },
			{ "renderOrder", renderOrder }
,		};
		std::string data = jsonData.dump();
		return data;
	}


	void Sprite::SetTexture(std::string newPath)
	{
		if (newPath != "")
		{
			// Save path for referencing later if needed
			path = newPath;
			Texture* newTexture = new Texture();
			if (newTexture->loadFromFile(newPath))
			{
				texture = newTexture->getTexture();
				textureWidth = (float)newTexture->getWidth();
				textureHeight = (float)newTexture->getHeight();

				// Set pivot point to the center of the texture by default
				offset = { textureWidth / 2, textureHeight / 2 };
			}
		}
		else
			texture = nullptr;
	}


	void Sprite::SetOffset(Vector2 newOffset)
	{
		offset = newOffset;
	}


	Vector2 Sprite::GetOffset()
	{
		return offset;
	}


	SDL_Texture* Sprite::GetTexture()
	{
		return texture;
	}


	float Sprite::GetTextureWidth()
	{
		return (float)textureWidth;
	}


	float Sprite::GetTextureHeight()
	{
		return (float)textureHeight;
	}

	
	std::string Sprite::GetPath()
	{
		return path;
	}

	void Sprite::SetRenderOrder(int order)
	{
		renderOrder = order;
	}

	int Sprite::GetRenderOrder()
	{
		return renderOrder;
	}

	void Sprite::RemoveTexture()
	{
		texture = NULL;
	}
}
