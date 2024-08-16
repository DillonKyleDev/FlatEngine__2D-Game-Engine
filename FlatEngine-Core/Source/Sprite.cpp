#include "Sprite.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Sprite::Sprite(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::T_Sprite);
		SetID(myID);
		SetParentID(parentID);
		m_texture = Texture();
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_scale = Vector2(1, 1);
		m_pivotPoint = Center;
		m_pivotOffset = Vector2(0, 0);
		m_offset = Vector2(0, 0);
		m_path = "";
		m_tintColor = Vector4(1, 1, 1, 1);
		m_renderOrder = 0;
	}

	Sprite::Sprite(Sprite* toCopy, long newParentID, long myID)
	{
		SetType(Component::ComponentTypes::T_Sprite);
		if (myID != -1)
			SetID(myID);
		else
			SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		m_texture = toCopy->m_texture;
		m_textureWidth = toCopy->m_textureHeight;
		m_textureHeight = toCopy->m_textureWidth;
		m_scale = toCopy->m_scale;
		m_pivotPoint = toCopy->m_pivotPoint;
		m_pivotOffset = toCopy->m_pivotOffset;
		m_offset = toCopy->m_offset;
		m_path = toCopy->m_path;
		m_tintColor = toCopy->m_tintColor;
		m_renderOrder = toCopy->m_renderOrder;
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
			{ "path", m_path },
			{ "xScale", m_scale.x },
			{ "yScale", m_scale.y },
			{ "pivotPoint", GetPivotPointString() },
			{ "xOffset", m_offset.x },
			{ "yOffset", m_offset.y },
			{ "tintColorX", m_tintColor.x },
			{ "tintColorY", m_tintColor.y },
			{ "tintColorZ", m_tintColor.z },
			{ "tintColorW", m_tintColor.w },
			{ "renderOrder", m_renderOrder }
,		};
		std::string data = jsonData.dump();
		return data;
	}

	void Sprite::SetTexture(std::string newPath)
	{
		RemoveTexture();

		if (newPath != "")
		{
			// Save path for referencing later if needed
			m_path = newPath;
			if (m_texture.LoadFromFile(newPath))
			{
				m_textureWidth = (float)m_texture.GetWidth();
				m_textureHeight = (float)m_texture.GetHeight();

				// Set pivot point to the center of the texture by default
				m_offset = { m_textureWidth / 2, m_textureHeight / 2 };
				m_pivotOffset = m_offset;
			}
			else
			{
				// Set broken texture Texture
				m_texture.LoadFromFile(F_ResourceFailedToLoadImagePath);
				if (m_textureWidth == 0 || m_textureHeight == 0)
				{
					m_textureWidth = 50;
					m_textureHeight = 50;
					SetOffset(Vector2(25, 25));
				}
			}
		}
		else
		{
			LogString("Sprite::SetTexture() - Texture could not be loaded");
			RemoveTexture();
		}
	}

	void Sprite::SetOffset(Vector2 newOffset)
	{
		m_offset = newOffset;
		SetScale(m_scale);
	}

	Vector2 Sprite::GetOffset()
	{
		return m_offset;
	}

	SDL_Texture* Sprite::GetTexture()
	{
		return m_texture.GetTexture();
	}

	void Sprite::SetScale(Vector2 newScale)
	{
		m_scale = newScale;
		switch (m_pivotPoint)
		{
			case PivotPoint::Center:
			{
				// Reset pivotOffset
				m_pivotOffset = m_offset;
				break;
			}
			case PivotPoint::Left:
			{
				m_pivotOffset = Vector2(0 + (m_offset.x * (1 / m_scale.x)), m_offset.y);
				break;
			}
			case PivotPoint::Right:
			{
				m_pivotOffset = Vector2((2 * m_offset.x) - m_offset.x * (1 / m_scale.x), m_offset.y);
				break;
			}
			case PivotPoint::Top:
			{
				m_pivotOffset = Vector2(m_offset.x, 0 + (m_offset.y * (1 / m_scale.y)));
				break;
			}
			case PivotPoint::Bottom:
			{
				m_pivotOffset = Vector2(m_offset.x, (2 * m_offset.y) - m_offset.y * (1 / m_scale.y));
				break;
			}

			case PivotPoint::TopLeft:
			{
				m_pivotOffset = Vector2(0 + (m_offset.x * (1 / m_scale.x)), 0 + (m_offset.y * (1 / m_scale.y)));
				break;
			}
			case PivotPoint::TopRight:
			{
				m_pivotOffset = Vector2((2 * m_offset.x) - m_offset.x * (1 / m_scale.x), 0 + (m_offset.y * (1 / m_scale.y)));
				break;
			}
			case PivotPoint::BottomLeft:
			{
				m_pivotOffset = Vector2(0 + (m_offset.x * (1 / m_scale.x)), (2 * m_offset.y) - m_offset.y * (1 / m_scale.y));
				break;
			}
			case PivotPoint::BottomRight:
			{
				m_pivotOffset = Vector2((2 * m_offset.x) - m_offset.x * (1 / m_scale.x), (2 * m_offset.y) - m_offset.y * (1 / m_scale.y));
				break;
			}
			default:
			{
				m_pivotOffset = m_offset;
				break;
			}
		}
	}

	Vector2 Sprite::GetScale()
	{
		return m_scale;
	}

	float Sprite::GetTextureWidth()
	{
		return m_textureWidth;
	}

	float Sprite::GetTextureHeight()
	{
		return m_textureHeight;
	}

	std::string Sprite::GetPath()
	{
		return m_path;
	}

	void Sprite::SetRenderOrder(int order)
	{
		m_renderOrder = order;
	}

	int Sprite::GetRenderOrder()
	{
		return m_renderOrder;
	}

	void Sprite::RemoveTexture()
	{
		m_texture.FreeTexture();
	}

	void Sprite::SetPivotPoint(PivotPoint newPivot)
	{
		m_pivotPoint = newPivot;
		SetScale(m_scale);
	}

	void Sprite::SetPivotPoint(std::string newPivot)
	{
		if (newPivot == "Center")
			m_pivotPoint = Sprite::PivotPoint::Center;
		else if (newPivot == "Left")
			m_pivotPoint = Sprite::PivotPoint::Left;
		else if (newPivot == "Right")
			m_pivotPoint = Sprite::PivotPoint::Right;
		else if (newPivot == "Top")
			m_pivotPoint = Sprite::PivotPoint::Top;
		else if (newPivot == "Bottom")
			m_pivotPoint = Sprite::PivotPoint::Bottom;
		else if (newPivot == "TopLeft")
			m_pivotPoint = Sprite::PivotPoint::TopLeft;
		else if (newPivot == "TopRight")
			m_pivotPoint = Sprite::PivotPoint::TopRight;
		else if (newPivot == "BottomLeft")
			m_pivotPoint = Sprite::PivotPoint::BottomLeft;
		else if (newPivot == "BottomRight")
			m_pivotPoint = Sprite::PivotPoint::BottomRight;
		else
			m_pivotPoint = Sprite::PivotPoint::Center;

		SetScale(m_scale);
	}

	Sprite::PivotPoint Sprite::GetPivotPoint()
	{
		return m_pivotPoint;
	}

	std::string Sprite::GetPivotPointString()
	{
		std::string pivotString;
		switch (m_pivotPoint)
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
		m_pivotOffset = newPivotOffset;
	}

	Vector2 Sprite::GetPivotOffset()
	{
		return m_pivotOffset;
	}

	void Sprite::SetTintColor(Vector4 newTintColor)
	{
		m_tintColor = newTintColor;
	}

	Vector4 Sprite::GetTintColor()
	{
		return m_tintColor;
	}
}
