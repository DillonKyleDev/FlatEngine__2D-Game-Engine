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
		TTF_Font* m_font;
		std::string m_fontPath;
		int m_fontSize;
		std::string m_text;
		SDL_Color m_color;
		Texture m_texture;
		Vector2 m_offset;
		int m_renderOrder;
	};
}

