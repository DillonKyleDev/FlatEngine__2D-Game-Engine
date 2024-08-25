#pragma once
#include "Vector2.h"
#include "Texture.h"

#include <string>
#include <map>


namespace FlatEngine
{
	class TileSet
	{
	public:
		TileSet();
		~TileSet();

		std::string GetData();
		void SetName(std::string name);
		std::string GetName();
		void SetTexturePath(std::string texturePath);
		std::string GetTexturePath();
		void SetTileSetPath(std::string tileSetPath);
		std::string GetTileSetPath();
		Texture* GetTexture();
		std::map<int, std::pair<Vector2, Vector2>> GetTileSet();
		void SetTileWidth(int width);
		void SetTileHeight(int height);
		int GetTileWidth();
		int GetTileHeight();
		void SaveCopyOfTexture();
		void InitializeUVs();
		void ToggleTile(int index);

	private:
		std::string m_name;
		std::string m_texturePath;
		std::string m_tileSetPath;
		Texture m_texture;
		std::map<int, std::pair<Vector2, Vector2>> m_allTileUVs;
		std::map<int, std::pair<Vector2, Vector2>> m_tileSetUVs;
		int m_tileWidth;
		int m_tileHeight;
	};
}