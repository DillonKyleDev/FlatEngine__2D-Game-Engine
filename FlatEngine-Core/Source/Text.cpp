#include "Text.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::T_Text);
		SetID(myID);
		SetParentID(parentID);
		fontPath = "assets/fonts/Cinzel/Cinzel-Black.ttf";
		fontSize = 40;
		font = TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40);
		color = { 1,1,1 };
		text = "Text Component";
		renderOrder = 0;
		texture = Texture();
		texture.LoadFromRenderedText("Text Component", { 1,1,1 }, TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40));
		offset = Vector2((float)texture.GetWidth() / 2, (float)texture.GetHeight() / 2);
	}

	Text::Text(Text* toCopy, long newParentID, long myID)
	{
		SetType(Component::ComponentTypes::T_Text);
		if (myID != -1)
			SetID(myID);
		else
			SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		fontPath = toCopy->GetFontPath();
		fontSize = toCopy->GetFontSize();
		font = TTF_OpenFont(fontPath.c_str(), fontSize); // Maybe just use global fonts for this instead
		color = toCopy->GetColor();
		text = toCopy->GetText();
		renderOrder = toCopy->GetRenderOrder();
		texture = Texture();
		// This might not work may need to be changed back to original constructor or messed with
		texture.LoadFromRenderedText(text.c_str(), color, font);
		offset = Vector2((float)texture.GetWidth() / 2, (float)texture.GetHeight() / 2);
	}

	Text::~Text()
	{
		font = nullptr;
		TTF_CloseFont(font);
	}

	void Text::LoadText()
	{
		texture.LoadFromRenderedText(text, color, font);
		offset = Vector2((float)texture.GetWidth() / 2, (float)texture.GetHeight() / 2);
	}

	void Text::SetRenderOrder(int order)
	{
		renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return renderOrder;
	}

	Texture Text::GetTexture()
	{
		return texture;
	}

	void Text::SetFontPath(std::string path)
	{
		fontPath = path;
	}

	std::string Text::GetFontPath()
	{
		return fontPath;
	}

	void Text::SetFontSize(int newFontSize)
	{
		fontSize = newFontSize;
	}

	int Text::GetFontSize()
	{
		return fontSize;
	}

	void Text::SetColor(Vector4 newColor)
	{
		color.r = (Uint8)newColor.x;
		color.g = (Uint8)newColor.y;
		color.b = (Uint8)newColor.z;
		color.a = (Uint8)newColor.w;
	}

	SDL_Color Text::GetColor()
	{
		return color;
	}

	void Text::SetText(std::string newText)
	{
		text = newText;
	}

	std::string Text::GetText()
	{
		return text;
	}

	void Text::SetOffset(Vector2 newOffset)
	{
		offset = newOffset;
	}

	Vector2 Text::GetOffset()
	{
		return offset;
	}

	std::string Text::GetData()
	{
		json jsonData = {
			{ "type", "Text" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "fontPath", fontPath },
			{ "text", text },
			{ "fontSize", fontSize },
			{ "f_red", color.r },
			{ "f_green", color.g },
			{ "f_blue", color.b },
			{ "f_alpha", color.a },
			{ "xOffset", offset.x },
			{ "yOffset", offset.y },
			{ "renderOrder", renderOrder },
		};
		std::string data = jsonData.dump();
		return data;
	}
}