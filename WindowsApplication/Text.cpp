#include "Text.h"



namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		this->SetType(Component::ComponentTypes::Text);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->fontPath = "assets/fonts/Cinzel/Cinzel-Black.ttf";
		this->fontSize = 40;
		this->font = TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40);
		this->color = { 1,1,1 };
		this->text = "Text Component";
		this->renderOrder = 0;
		this->texture = std::make_shared<Texture>();
		this->texture->loadFromRenderedText("Text Component", { 1,1,1 }, TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 40));
		this->offset = Vector2((float)this->texture->getWidth() / 2, (float)this->texture->getHeight() / 2);
	}

	Text::~Text()
	{
		this->font = nullptr;
		this->texture = nullptr;
		TTF_CloseFont(this->font);
	}

	void Text::LoadText()
	{
		this->texture->loadFromRenderedText(this->text, this->color, this->font);
		this->offset = Vector2((float)this->texture->getWidth() / 2, (float)this->texture->getHeight() / 2);
	}

	void Text::SetRenderOrder(int order)
	{
		this->renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return this->renderOrder;
	}

	std::shared_ptr<Texture> Text::GetTexture()
	{
		return this->texture;
	}

	void Text::SetFontPath(std::string path)
	{
		this->fontPath = path;
	}

	std::string Text::GetFontPath()
	{
		return this->fontPath;
	}

	void Text::SetFontSize(int fontSize)
	{
		this->fontSize = fontSize;
	}

	int Text::GetFontSize()
	{
		return this->fontSize;
	}

	void Text::SetColor(SDL_Color color)
	{
		this->color = color;
	}

	SDL_Color Text::GetColor()
	{
		return this->color;
	}

	void Text::SetText(std::string text)
	{
		this->text = text;
	}

	std::string Text::GetText()
	{
		return this->text;
	}

	void Text::SetOffset(Vector2 offset)
	{
		this->offset = offset;
	}

	Vector2 Text::GetOffset()
	{
		return this->offset;
	}

	std::string Text::GetData()
	{
		json jsonData = {
			{ "type", "Text" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "fontPath", this->fontPath },
			{ "text", this->text },
			{ "fontSize", this->fontSize },
			{ "f_red", this->color.r },
			{ "f_green", this->color.g },
			{ "f_blue", this->color.b },
			{ "f_alpha", this->color.a },
			{ "xOffset", this->offset.x },
			{ "yOffset", this->offset.y },
			{ "renderOrder", this->renderOrder },
		};
		std::string data = jsonData.dump();
		return data;
	}
}