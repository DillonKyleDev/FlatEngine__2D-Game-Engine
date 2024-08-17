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
		std::shared_ptr<Texture> GetTexture(std::string textureName);
		std::string GetPath(std::string textureName);
		Texture *m_errorTexture;
	private:
		std::map<std::string, Vector4> m_colors;
		std::map<std::string, std::shared_ptr<Texture>> m_textures;
		std::map<std::string, std::string> m_paths;
		std::string m_resourceFailedToLoadImagePath;

	};
}