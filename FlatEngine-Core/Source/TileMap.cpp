#include "TileMap.h"
#include "TileSet.h"


namespace FlatEngine
{
	FlatEngine::TileMap::TileMap(long myID, long parentID)
	{
		SetType(ComponentTypes::T_TileMap);
		SetID(myID);
		SetParentID(parentID);
		m_width = 20;
		m_height = 20;
		m_tileWidth = 16;
		m_tileHeight = 16;
		m_storedTextureUVs = std::map<int, std::pair<std::string, int>>();
		m_indexedTextureUVs = std::map<int, std::pair<SDL_Texture*, std::pair<Vector2, Vector2>>>();
		m_selectedTileSet = "";
		m_tileSetNames = std::vector<std::string>();
	}

	TileMap::TileMap(TileMap* toCopy, long myID, long newParentID)
	{
		SetType(ComponentTypes::T_TileMap);
		SetID(myID);
		SetParentID(newParentID);
		m_width = toCopy->m_width;
		m_height = toCopy->m_height;
		m_tileWidth = toCopy->m_tileWidth;
		m_tileHeight = toCopy->m_tileHeight;
		m_storedTextureUVs = toCopy->m_storedTextureUVs;
		m_indexedTextureUVs = toCopy->m_indexedTextureUVs;
		m_selectedTileSet = toCopy->m_selectedTileSet;
		m_tileSetNames = toCopy->m_tileSetNames;
	}

	TileMap::~TileMap()
	{
	}

	std::string TileMap::GetData()
	{
		std::map<int, std::pair<std::string, int>> m_storedTextureUVs;
		json tileSetNames = json::array();
		json indexData = json::array();

		for (std::string tileSetName : m_tileSetNames)
		{
			json name = {
				{ "name", tileSetName }
			};
			tileSetNames.push_back(name);
		}

		for (std::pair<int, std::pair<std::string, int>> data : m_storedTextureUVs)
		{
			json index = {
				{ "index", { data.first, { data.second.first, data.second.second } } }
			};
			indexData.push_back(index);
		}

		json jsonData = {
			{ "type", "TileMap" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "width", m_width },
			{ "height", m_height },
			{ "tileWidth", m_tileWidth },
			{ "tileHeight", m_tileHeight },
			{ "tileSets", tileSetNames },
			{ "indexData", indexData }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	int TileMap::GetWidth()
	{
		return m_width;
	}

	int TileMap::GetHeight()
	{
		return m_height;
	}

	int TileMap::GetTileWidth()
	{
		return m_tileWidth;
	}

	int TileMap::GetTileHeight()
	{
		return m_tileHeight;
	}

	void TileMap::SetTileWidth(int width)
	{
		m_tileWidth = width;
	}

	void TileMap::SetTileHeight(int height)
	{
		m_tileHeight = height;
	}

	void TileMap::SetWidth(int width)
	{
		m_width = width;
	}

	void TileMap::SetHeight(int height)
	{
		m_height = height;
	}

	std::string TileMap::GetSelectedTileSet()
	{
		return m_selectedTileSet;
	}

	void TileMap::SetSelectedTileSet(std::string tileSet)
	{
		m_selectedTileSet = tileSet;
	}

	void TileMap::AddTileSet(std::string name)
	{
		m_tileSetNames.push_back(name);
	}

	void TileMap::RemoveTileSet(std::string name)
	{
		for (std::vector<std::string>::iterator iter = m_tileSetNames.begin(); iter != m_tileSetNames.end(); iter++)
		{
			if (name == *iter)
				m_tileSetNames.erase(iter);
		}
	}

	void TileMap::SetTileSets(std::vector<std::string> tileSets)
	{
		m_tileSetNames = tileSets;
	}

	std::vector<std::string> TileMap::GetTileSets()
	{
		return m_tileSetNames;
	}

	std::map<int, std::pair<SDL_Texture*, std::pair<Vector2, Vector2>>> TileMap::GetIndexedTiles()
	{
		return m_indexedTextureUVs;
	}

	void TileMap::SetTile(int tileMapIndex, TileSet* tileSet, int tileSetIndex)
	{
		std::pair<std::string, int> newPair = { tileSet->GetName(), tileSetIndex};
		if (m_storedTextureUVs.count(tileMapIndex) == 0)
		{
			m_storedTextureUVs.emplace(tileMapIndex, newPair);
		}
		else
		{
			m_storedTextureUVs.at(tileMapIndex) = newPair;
		}

		SDL_Texture* texture = tileSet->GetTexture()->GetTexture();
		Vector2 uvStart = tileSet->GetIndexUVs(tileSetIndex).first;
		Vector2 uvEnd = tileSet->GetIndexUVs(tileSetIndex).second;
		std::pair<SDL_Texture*, std::pair<Vector2, Vector2>> indexedPair = { texture, { uvStart, uvEnd } };
		if (m_indexedTextureUVs.count(tileMapIndex) > 0)
		{
			m_indexedTextureUVs.at(tileMapIndex) = indexedPair;
		}
		else
			m_indexedTextureUVs.emplace(tileMapIndex, indexedPair);
	}

	void TileMap::CreateTileMap()
	{
		m_indexedTextureUVs.clear();

		for (std::pair<int, std::pair<std::string, int>> indexData : m_storedTextureUVs)
		{
			TileSet* tileSet = GetTileSet(indexData.second.first);								// Get the TileSet
			SDL_Texture* texture = tileSet->GetTexture()->GetTexture();							// Get the TileSets SDL_Texture*
			std::pair<Vector2,Vector2> UVs = tileSet->GetIndexUVs(indexData.second.second);		// Get the UVs for the TileSet index
			std::pair<SDL_Texture*, std::pair<Vector2, Vector2>> newPair = { texture, UVs };	// Create a new pair with all of that data

			m_indexedTextureUVs.emplace(indexData.first, newPair);								// Save the data for fast access at runtime
		}
	}
}