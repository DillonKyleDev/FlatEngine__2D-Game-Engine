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
		pivotPoint = Center;
		pivotOffset = Vector2(0, 0);
		offset = Vector2(0, 0);
		path = "";
		tintColor = Vector4(1, 1, 1, 1);
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
		pivotPoint = toCopy->pivotPoint;
		pivotOffset = toCopy->pivotOffset;
		offset = toCopy->offset;
		path = toCopy->path;
		tintColor = toCopy->tintColor;
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
			{ "pivotPoint", GetPivotPointString() },
			{ "xOffset", offset.x },
			{ "yOffset", offset.y },
			{ "tintColorX", tintColor.x },
			{ "tintColorY", tintColor.y },
			{ "tintColorZ", tintColor.z },
			{ "tintColorW", tintColor.w },
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
				pivotOffset = offset;
			}
		}
		else
			texture = nullptr;
	}


	void Sprite::SetOffset(Vector2 newOffset)
	{
		offset = newOffset;
		SetScale(scale);
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
		switch (pivotPoint) 
		{
			case PivotPoint::Center:
			{
				// Reset pivotOffset
				pivotOffset = offset;
				break;
			}
			case PivotPoint::Left:
			{
				pivotOffset = Vector2(0 + (offset.x * (1 / scale.x)), offset.y);
				break;
			}
			case PivotPoint::Right:
			{
				pivotOffset = Vector2((2 * offset.x) - offset.x * (1 / scale.x), offset.y);
				break;
			}
			case PivotPoint::Top:
			{
				pivotOffset = Vector2(offset.x, 0 + (offset.y * (1 / scale.y)));
				break;
			}
			case PivotPoint::Bottom:
			{
				pivotOffset = Vector2(offset.x, (2 * offset.y) - offset.y * (1 / scale.y));
				break;
			}

			case PivotPoint::TopLeft:
			{
				pivotOffset = Vector2(0 + (offset.x * (1 / scale.x)), 0 + (offset.y * (1 / scale.y)));
				break;
			}
			case PivotPoint::TopRight:
			{
				pivotOffset = Vector2((2 * offset.x) - offset.x * (1 / scale.x), 0 + (offset.y * (1 / scale.y)));
				break;
			}
			case PivotPoint::BottomLeft:
			{
				pivotOffset = Vector2(0 + (offset.x * (1 / scale.x)), (2 * offset.y) - offset.y * (1 / scale.y));
				break;
			}
			case PivotPoint::BottomRight:
			{
				pivotOffset = Vector2((2 * offset.x) - offset.x * (1 / scale.x), (2 * offset.y) - offset.y * (1 / scale.y));
				break;
			}
			default:
			{
				pivotOffset = offset;
				break;
			}
		}
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

	void Sprite::SetPivotPoint(PivotPoint newPivot)
	{
		pivotPoint = newPivot;
		SetScale(scale);
	}

	void Sprite::SetPivotPoint(std::string newPivot)
	{
		if (newPivot == "Center")
			pivotPoint = Sprite::PivotPoint::Center;
		else if (newPivot == "Left")
			pivotPoint = Sprite::PivotPoint::Left;
		else if (newPivot == "Right")
			pivotPoint = Sprite::PivotPoint::Right;
		else if (newPivot == "Top")
			pivotPoint = Sprite::PivotPoint::Top;
		else if (newPivot == "Bottom")
			pivotPoint = Sprite::PivotPoint::Bottom;
		else if (newPivot == "TopLeft")
			pivotPoint = Sprite::PivotPoint::TopLeft;
		else if (newPivot == "TopRight")
			pivotPoint = Sprite::PivotPoint::TopRight;
		else if (newPivot == "BottomLeft")
			pivotPoint = Sprite::PivotPoint::BottomLeft;
		else if (newPivot == "BottomRight")
			pivotPoint = Sprite::PivotPoint::BottomRight;
		else
			pivotPoint = Sprite::PivotPoint::Center;

		SetScale(scale);
	}

	Sprite::PivotPoint Sprite::GetPivotPoint()
	{
		return pivotPoint;
	}

	std::string Sprite::GetPivotPointString()
	{
		std::string pivotString;
		switch (pivotPoint)
		{
		case Sprite::PivotPoint::Left:
			pivotString = "Left";
			break;
		case Sprite::PivotPoint::Right:
			pivotString = "Right";
			break;
		case Sprite::PivotPoint::Top:
			pivotString = "Top";
			break;
		case Sprite::PivotPoint::Bottom:
			pivotString = "Bottom";
			break;
		case Sprite::PivotPoint::TopLeft:
			pivotString = "TopLeft";
			break;
		case Sprite::PivotPoint::TopRight:
			pivotString = "TopRight";
			break;
		case Sprite::PivotPoint::BottomLeft:
			pivotString = "BottomLeft";
			break;
		case Sprite::PivotPoint::BottomRight:
			pivotString = "BottomRight";
			break;
		default:
			pivotString = "Center";
			break;
		}

		return pivotString;
	}

	void Sprite::SetPivotOffset(Vector2 newPivotOffset)
	{
		pivotOffset = newPivotOffset;
	}

	Vector2 Sprite::GetPivotOffset()
	{
		return pivotOffset;
	}

	void Sprite::SetTintColor(Vector4 newTintColor)
	{
		tintColor = newTintColor;
	}

	Vector4 Sprite::GetTintColor()
	{
		return tintColor;
	}
}
