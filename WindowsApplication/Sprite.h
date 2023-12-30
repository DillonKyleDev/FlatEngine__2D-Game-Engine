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
		Sprite();
		~Sprite();
		void SetTexture(std::string path);
		SDL_Texture* GetTexture();
		std::string GetPath();
		void RemoveTexture();
		std::string GetData();

	private:
		SDL_Texture* texture;
		std::string path;
	};
}
