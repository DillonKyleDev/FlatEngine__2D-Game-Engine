#include "Text.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::T_Text);
		SetID(myID);
		SetParentID(parentID);
		m_fontPath = "assets/fonts/Cinzel/Cinzel-Black.ttf";
		m_fontSize = 40;
		m_font = TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40);
		m_color = { 1,1,1 };
		m_text = "Text Component";
		m_renderOrder = 0;
		m_texture = Texture();
		m_texture.LoadFromRenderedText("Text Component", { 1,1,1 }, TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40));
		m_offset = Vector2((float)m_texture.GetWidth() / 2, (float)m_texture.GetHeight() / 2);
	}

	Text::~Text()
	{
		m_font = nullptr;
		TTF_CloseFont(m_font);
	}

	void Text::LoadText()
	{
		m_texture.LoadFromRenderedText(m_text, m_color, m_font);
		m_offset = Vector2((float)m_texture.GetWidth() / 2, (float)m_texture.GetHeight() / 2);
	}

	void Text::SetRenderOrder(int order)
	{
		m_renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return m_renderOrder;
	}

	Texture Text::GetTexture()
	{
		return m_texture;
	}

	void Text::SetFontPath(std::string path)
	{
		m_fontPath = path;
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
		m_color.r = (Uint8)newColor.x;
		m_color.g = (Uint8)newColor.y;
		m_color.b = (Uint8)newColor.z;
		m_color.a = (Uint8)newColor.w;
	}

	SDL_Color Text::GetColor()
	{
		return m_color;
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
			{ "f_red", m_color.r },
			{ "f_green", m_color.g },
			{ "f_blue", m_color.b },
			{ "f_alpha", m_color.a },
			{ "xOffset", m_offset.x },
			{ "yOffset", m_offset.y },
			{ "renderOrder", m_renderOrder },
		};
		std::string data = jsonData.dump();
		return data;
	}
}