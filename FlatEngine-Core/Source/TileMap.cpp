#include "TileMap.h"
#include "TileSet.h"
#include "BoxCollider.h"
#include "FlatEngine.h"
#include "Scene.h"


namespace FlatEngine
{
	FlatEngine::TileMap::TileMap(long myID, long parentID)
	{
		SetType(ComponentTypes::T_TileMap);
		SetID(myID);
		SetParentID(parentID);
		m_width = 10;
		m_height = 5;
		m_tileWidth = 16;
		m_tileHeight = 16;
		m_renderOrder = FL::F_maxSpriteLayers / 2;
		m_tiles = std::map<int, std::map<int, Tile>>();
		m_selectedTileSet = "";
		m_tileSetNames = std::vector<std::string>();
		m_collisionAreas = std::vector<CollisionAreaData>();
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
		m_renderOrder = toCopy->m_renderOrder;
		m_tiles = toCopy->m_tiles;
		m_selectedTileSet = toCopy->m_selectedTileSet;
		m_tileSetNames = toCopy->m_tileSetNames;
		m_collisionAreas = toCopy->m_collisionAreas;
	}

	TileMap::~TileMap()
	{
	}

	std::string TileMap::GetData()
	{
		json tileSetNames = json::array();
		json tileData = json::array();

		for (std::string tileSetName : m_tileSetNames)
		{
			json name = {
				{ "name", tileSetName }
			};
			tileSetNames.push_back(name);
		}

		for (std::pair<int, std::map<int, Tile>> row : m_tiles)
		{	
			for (std::pair<int, Tile> col : row.second)
			{
				Tile tile = col.second;

				json tileJson = {
						{ "tileCoordX", tile.tileCoord.x },
						{ "tileCoordY", tile.tileCoord.y },
						{ "tileSetName", tile.tileSetName },
						{ "tileSetIndex", tile.tileSetIndex }
				};
				tileData.push_back(tileJson);
			}
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
			{ "tiles", tileData }
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

	void TileMap::SetRenderOrder(int renderOrder)
	{
		m_renderOrder = renderOrder;
	}

	int TileMap::GetRenderOrder()
	{
		return m_renderOrder;
	}

	std::string TileMap::GetSelectedTileSet()
	{
		return m_selectedTileSet;
	}

	void TileMap::SetSelectedTileSet(std::string tileSet)
	{
		m_selectedTileSet = tileSet;
	}

	std::vector<CollisionAreaData>& TileMap::GetCollisionAreas()
	{
		return m_collisionAreas;
	}

	BoxCollider* TileMap::AddCollisionArea(std::string label)
	{
		long nextID = GetLoadedScene()->GetNextComponentID();
		BoxCollider boxCollider = BoxCollider(nextID, GetParentID());
		boxCollider.SetActive(true);
		boxCollider.SetCollapsed(false);
		BoxCollider* colliderPtr = GetLoadedScene()->AddBoxCollider(boxCollider, GetParentID());
		//colliderPtr->SetActiveDimensions(spriteDimensions.x, spriteDimensions.y);

		CollisionAreaData newCollisionData;
		newCollisionData.name = label;
		newCollisionData.collider = boxCollider;
		m_collisionAreas.push_back(newCollisionData);
		return colliderPtr;
	}

	bool TileMap::ContainsCollisionAreaLabel(std::string label)
	{
		bool b_containsAreaLabel = false;
		for (CollisionAreaData collisionData : m_collisionAreas)
		{
			if (collisionData.name == label)
			{
				b_containsAreaLabel = true;
			}
		}
		return b_containsAreaLabel;
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

	std::map<int, std::map<int, Tile>> TileMap::GetTiles()
	{
		return m_tiles;
	}

	void TileMap::SetTile(Vector2 coord, TileSet* tileSet, int tileSetIndex)
	{
		SDL_Texture* texture = tileSet->GetTexture()->GetTexture();
		Vector2 uvStart = tileSet->GetIndexUVs(tileSetIndex).first;
		Vector2 uvEnd = tileSet->GetIndexUVs(tileSetIndex).second;

		Tile newTile = Tile();
		newTile.tileCoord = coord;
		int x = (int)coord.x;
		int y = (int)coord.y;
		newTile.tileSetName = tileSet->GetName();
		newTile.tileSetIndex = tileSetIndex;
		newTile.tileSetTexture = texture;
		newTile.uvStart = uvStart;
		newTile.uvEnd = uvEnd;

		if (m_tiles.count(x) > 0 && m_tiles.at(x).count(y) > 0)
		{
			m_tiles.at(x).at(y) = newTile;
		}
		else if (m_tiles.count(x) > 0 && m_tiles.at(x).count(y) == 0)
		{
			std::pair<int, Tile> newPair = { y, newTile };
			m_tiles.at(x).emplace(newPair);
		}
		else if (m_tiles.count(x) == 0)
		{
			std::pair<int, Tile> newPair = { y, newTile };
			std::map<int, Tile> yCoords;
			yCoords.emplace(newPair);
			m_tiles.emplace(x, yCoords);
		}
	}

	void TileMap::EraseTile(Vector2 tileMapCoords)
	{
		int x = (int)tileMapCoords.x;
		int y = (int)tileMapCoords.y;

		if (m_tiles.count(x) > 0 && m_tiles.at(x).count(y) > 0)
		{
			m_tiles.at(x).erase(y);
		}
	}
}