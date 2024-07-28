#pragma once
#include "Component.h"
#include "Texture.h"
#include "Vector4.h"

#include <SDL_ttf.h>


namespace FlatEngine
{
	class Text : public Component
	{
	public:
		Text(long myID = -1, long parentID = -1);
		Text(Text* toCopy, long newParentID, long myID = -1);
		~Text();

		void LoadText();
		void SetRenderOrder(int order);
		int GetRenderOrder();
		Texture GetTexture();
		void SetFontPath(std::string path);
		std::string GetFontPath();
		void SetFontSize(int fontSize);
		int GetFontSize();
		void SetColor(Vector4 newColor);
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
		Texture texture;
		Vector2 offset;
		int renderOrder;
	};
}

