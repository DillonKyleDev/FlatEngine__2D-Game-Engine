#include "Text.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		SetType(Component::ComponentTypes::Text);
		SetID(myID);
		SetParentID(parentID);
		fontPath = "assets/fonts/Cinzel/Cinzel-Black.ttf";
		fontSize = 40;
		font = TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40);
		color = { 1,1,1 };
		text = "Text Component";
		renderOrder = 0;
		texture = std::make_shared<Texture>();
		texture->loadFromRenderedText("Text Component", { 1,1,1 }, TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40));
		offset = Vector2((float)texture->getWidth() / 2, (float)texture->getHeight() / 2);
	}

	Text::Text(std::shared_ptr<Text> toCopy, long newParentID)
	{
		SetType(Component::ComponentTypes::Text);
		this->SetID(GetNextComponentID());
		SetParentID(newParentID);
		fontPath = toCopy->GetFontPath();
		fontSize = toCopy->GetFontSize();
		font = TTF_OpenFont(fontPath.c_str(), fontSize);
		color = toCopy->GetColor();
		text = toCopy->GetText();
		renderOrder = toCopy->GetRenderOrder();
		texture = std::make_shared<Texture>();
		// This might not work may need to be changed back to original constructor or messed with
		texture->loadFromRenderedText(text.c_str(), color, font);
		offset = Vector2((float)texture->getWidth() / 2, (float)texture->getHeight() / 2);
	}

	Text::~Text()
	{
		font = nullptr;
		texture = nullptr;
		TTF_CloseFont(font);
	}

	void Text::LoadText()
	{
		texture->loadFromRenderedText(text, color, font);
		offset = Vector2((float)texture->getWidth() / 2, (float)texture->getHeight() / 2);
	}

	void Text::SetRenderOrder(int order)
	{
		renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return renderOrder;
	}

	std::shared_ptr<Texture> Text::GetTexture()
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

	void Text::SetFontSize(int fontSize)
	{
		fontSize = fontSize;
	}

	int Text::GetFontSize()
	{
		return fontSize;
	}

	void Text::SetColor(SDL_Color color)
	{
		color = color;
	}

	SDL_Color Text::GetColor()
	{
		return color;
	}

	void Text::SetText(std::string text)
	{
		text = text;
	}

	std::string Text::GetText()
	{
		return text;
	}

	void Text::SetOffset(Vector2 offset)
	{
		offset = offset;
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