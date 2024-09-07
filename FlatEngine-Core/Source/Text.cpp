#include "Text.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::T_Text);
		SetID(myID);
		SetParentID(parentID);
		m_fontPath = GetFilePath("cinzelBlack");
		m_fontSize = 40;
		m_font = TTF_OpenFont(m_fontPath.c_str(), 40);
		m_tintColor = Vector4(1,1,1,1);
		m_text = "Sample Text";
		m_renderOrder = 0;
		m_texture = std::make_shared<Texture>();
		m_white.r = (Uint8)255;
		m_white.g = (Uint8)255;
		m_white.b = (Uint8)255;
		m_white.a = (Uint8)255;

		m_texture->LoadFromRenderedText(m_text.c_str(), m_white, m_font);
		m_offset = Vector2((float)m_texture->GetWidth() / 2, (float)m_texture->GetHeight() / 2);
	}

	Text::~Text()
	{
		m_font = nullptr;
		TTF_CloseFont(m_font);
	}

	void Text::LoadText()
	{
		m_texture->LoadFromRenderedText(m_text, m_white, m_font);
		m_offset = Vector2((float)m_texture->GetWidth() / 2, (float)m_texture->GetHeight() / 2);
	}

	void Text::SetRenderOrder(int order)
	{
		m_renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return m_renderOrder;
	}

	std::shared_ptr<Texture> Text::GetTexture()
	{
		return m_texture;
	}

	void Text::SetFontPath(std::string path)
	{
		m_fontPath = path;
		m_font = TTF_OpenFont(m_fontPath.c_str(), 40);
		if (m_font != nullptr)
		{
			LoadText();
		}
	}

	std::string Text::GetFontPath()
	{
		return m_fontPath;
	}

	void Text::SetFontSize(int newFontSize)
	{
		m_fontSize = newFontSize;
	}

	int Text::GetFontSize()
	{
		return m_fontSize;
	}

	void Text::SetColor(Vector4 newColor)
	{
		m_tintColor = newColor;
	}

	Vector4 Text::GetColor()
	{
		return m_tintColor;
	}

	void Text::SetText(std::string newText)
	{
		m_text = newText;
	}

	std::string Text::GetText()
	{
		return m_text;
	}

	void Text::SetOffset(Vector2 newOffset)
	{
		m_offset = newOffset;
	}

	Vector2 Text::GetOffset()
	{
		return m_offset;
	}

	std::string Text::GetData()
	{
		json jsonData = {
			{ "type", "Text" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "fontPath", m_fontPath },
			{ "text", m_text },
			{ "fontSize", m_fontSize },
			{ "tintColorX", m_tintColor.x },
			{ "tintColorY", m_tintColor.y },
			{ "tintColorZ", m_tintColor.z },
			{ "tintColorW", m_tintColor.w },
			{ "xOffset", m_offset.x },
			{ "yOffset", m_offset.y },
			{ "renderOrder", m_renderOrder },
		};
		std::string data = jsonData.dump();
		return data;
	}
}