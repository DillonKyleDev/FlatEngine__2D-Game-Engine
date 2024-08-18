//////////////////////
//Global      Assets//
//  //////VV///////  /
//     UUUUUUUU     //
///\-------------/////
//////////////////////
#include <map>
#include <string>
#include "Texture.h"

class Vector4;

namespace FlatEngine
{
	class Texture;

	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();
		void CollectColors();
		void CollectTextures();
		Vector4 GetColor(std::string colorName);
		std::shared_ptr<Texture>GetTextureObject(std::string textureName);
		SDL_Texture* GetTexture(std::string textureName);		
	private:
		std::map<std::string, Vector4> m_colors;
		std::map<std::string, std::shared_ptr<Texture>> m_textures;		
		std::string m_resourceFailedToLoadImagePath;
		Texture m_errorTexture;
	};
}