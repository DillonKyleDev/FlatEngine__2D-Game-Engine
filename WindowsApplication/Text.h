#pragma once
#include "Component.h"
#include "Texture.h"
#include <SDL_ttf.h>


namespace FlatEngine
{
	class Text : public Component
	{
	public:
		Text(long myID = -1, long parentID = -1);
		Text(std::shared_ptr<Text> toCopy, long newParentID);
		~Text();

		void LoadText();
		void SetRenderOrder(int order);
		int GetRenderOrder();
		std::shared_ptr<Texture> GetTexture();
		void SetFontPath(std::string path);
		std::string GetFontPath();
		void SetFontSize(int fontSize);
		int GetFontSize();
		void SetColor(SDL_Color color);
		SDL_Color GetColor();
		void SetText(std::string text);
		std::string GetText();
		void SetOffset(Vector2 offset);
		Vector2 GetOffset();
		std::string GetData();
		
	private:
		TTF_Font* font;
		std::string fontPath;
		int fontSize;
		std::string text;
		SDL_Color color;
		std::shared_ptr<Texture> texture;
		Vector2 offset;
		int renderOrder;
	};
}

