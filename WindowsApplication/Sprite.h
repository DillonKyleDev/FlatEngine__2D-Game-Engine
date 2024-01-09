#pragma once
#include "Component.h"
#include "Texture.h"
#include <SDL.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Sprite : public Component
	{
	public:
		Sprite(long myID, long parentID);
		~Sprite();
		void SetTexture(std::string path);
		SDL_Texture* GetTexture();
		float GetTextureWidth();
		float GetTextureHeight();
		std::string GetPath();
		void SetOffset(Vector2 offset);
		Vector2 GetOffset();
		void RemoveTexture();
		std::string GetData();

	private:
		SDL_Texture* texture;
		float textureWidth;
		float textureHeight;
		Vector2 offset;
		std::string path;
	};
}
