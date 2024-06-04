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
		scale = Vector2(1, 1);
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
		texture = toCopy->texture;
		textureWidth = toCopy->textureHeight;
		textureHeight = toCopy->textureWidth;
		scale = toCopy->scale;
		offset = toCopy->offset;
		path = toCopy->path;
		renderOrder = toCopy->renderOrder;
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
			{ "path", path },
			{ "xScale", scale.x },
			{ "yScale", scale.y },
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

	void Sprite::SetScale(Vector2 newScale)
	{
		scale = newScale;
	}

	Vector2 Sprite::GetScale()
	{
		return scale;
	}


	float Sprite::GetTextureWidth()
	{
		return textureWidth;
	}


	float Sprite::GetTextureHeight()
	{
		return textureHeight;
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
