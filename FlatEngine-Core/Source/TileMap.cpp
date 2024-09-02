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
		m_tempMoveTiles = std::map<int, std::map<int, Tile>>();
		m_selectedTileSet = "";
		m_tileSetNames = std::vector<std::string>();
		m_selectedCollisionArea = "";
		m_collisionAreas = std::map<std::string, std::vector<CollisionAreaData>>();	
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
		m_tempMoveTiles = toCopy->m_tempMoveTiles;
		m_selectedTileSet = toCopy->m_selectedTileSet;
		m_tileSetNames = toCopy->m_tileSetNames;
		m_selectedCollisionArea = toCopy->m_selectedCollisionArea;
		m_collisionAreas = toCopy->m_collisionAreas;		
	}

	TileMap::~TileMap()
	{
	}

	std::string TileMap::GetData()
	{
		json tileSetNames = json::array();
		json tileData = json::array();
		json collisionAreaData = json::array();

		// TileSet names
		for (std::string tileSetName : m_tileSetNames)
		{
			json name = {
				{ "name", tileSetName }
			};
			tileSetNames.push_back(name);
		}
		// Tile data
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
		// Collision Area data
		for (std::pair<std::string, std::vector<CollisionAreaData>> colData : m_collisionAreas)
		{			
			json areaCoords = json::array();
		
			for (CollisionAreaData data : colData.second)
			{
				Vector2 startCoord = data.startCoord;
				Vector2 endCoord = data.endCoord;

				json colData = {				
					{ "startCoordX", startCoord.x },
					{ "startCoordY", startCoord.y },
					{ "endCoordX", endCoord.x },
					{ "endCoordY", endCoord.y }
				};
				areaCoords.push_back(colData);
			}

			json colAreaData = {
				{ "name", colData.first },
				{ "areaData", areaCoords }
			};

			collisionAreaData.push_back(colAreaData);
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
			{ "tiles", tileData },
			{ "collisionAreas", collisionAreaData }
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

	std::map<std::string, std::vector<CollisionAreaData>>& TileMap::GetCollisionAreas()
	{
		return m_collisionAreas;
	}

	BoxCollider* TileMap::AddCollisionArea(std::string label, Vector2 startCoord, Vector2 endCoord)
	{
		BoxCollider newCollisionArea = BoxCollider(GetNextComponentID(), GetParentID());

		newCollisionArea.SetTileMapCollider(true);
		newCollisionArea.SetActive(true);
		newCollisionArea.SetCollapsed(false);
		newCollisionArea.SetIsContinuous(false);
		newCollisionArea.SetIsStatic(true);
		newCollisionArea.SetIsSolid(true);
		newCollisionArea.SetActiveLayer(0);
		newCollisionArea.SetRotation(0);
		newCollisionArea.SetShowActiveRadius(false);
		newCollisionArea.SetIsComposite(false);
		BoxCollider* colliderPtr = GetLoadedScene()->AddBoxCollider(newCollisionArea, GetParentID());

		CollisionAreaData colData;
		colData.collider = colliderPtr;
		colData.startCoord = startCoord;
		colData.endCoord = endCoord;


		if (m_collisionAreas.count(label) == 0)
		{
			std::vector<CollisionAreaData> colVector;
			colVector.push_back(colData);			
			m_collisionAreas.emplace(label, colVector);
		}
		else
		{
			m_collisionAreas.at(label).push_back(colData);
		}

		return colliderPtr;
	}

	//void TileMap::RemoveCollisionArea(std::string label, Vector2 startCoord, Vector2 endCoord)
	//{
	//	if (m_collisionAreas.count(label) > 0)
	//	{
	//		for (std::pair<Vector2, Vector2> coordVector : m_collisionAreaCoords.at(label))
	//		{
	//			if (((coordVector.first.x == startCoord.x && coordVector.first.y == startCoord.y) || (coordVector.first.x == endCoord.x && coordVector.first.y == endCoord.y)) &&
	//				((coordVector.second.x == startCoord.x && coordVector.second.y == startCoord.y) || (coordVector.second.x == endCoord.x && coordVector.second.y == endCoord.y)))
	//			{
	//				m_collisionAreas.erase(label);
	//				break;
	//			}
	//		}
	//	}
	//}

	void TileMap::SetCollisionAreaValues(std::string label, std::vector<std::pair<Vector2, Vector2>> colCoords)
	{
		if (m_collisionAreas.count(label) == 0)
		{
			std::vector<CollisionAreaData> collAreaData = std::vector<CollisionAreaData>();
			m_collisionAreas.emplace(label, collAreaData);
		}

		// Delete old BoxColliders
		for (CollisionAreaData collisionArea : m_collisionAreas.at(label))
		{
			GetLoadedScene()->RemoveComponent(collisionArea.collider);
		}
		m_collisionAreas.at(label).clear();

		// Use the passed coords to create new CollisionAreaDatas in this Collision Area
		for (std::pair<Vector2, Vector2> coord : colCoords)
		{
			// Create CollisionAreaData object with that data, including BoxCollider
			BoxCollider* collider = AddCollisionArea(label, coord.first, coord.second);

			// Use that data to set the positioning of the BoxCollider relative to the TileMap
			Vector2 colliderStart = Vector2(coord.first.x + 1, coord.first.y + 1);
			Vector2 colliderEnd = Vector2(coord.second.x + 1, coord.second.y + 1);

			float gridWidthsInATile = m_tileWidth / FL::F_pixelsPerGridSpace;
			float gridHeightsInATile = m_tileHeight / FL::F_pixelsPerGridSpace;

			Vector2 offset = Vector2();

			float colWidthInTiles = colliderEnd.x - colliderStart.x + 1;
			if (colliderEnd.x < colliderStart.x)
			{
				colWidthInTiles = colliderStart.x - colliderEnd.x + 1;
			}
			if (colWidthInTiles < 0)
			{
				colWidthInTiles *= -1;
			}
			float colHeightInTiles = colliderEnd.y - colliderStart.y + 1;
			if (colliderEnd.y < colliderStart.y)
			{
				colHeightInTiles = colliderStart.y - colliderEnd.y + 1;
			}
			if (colHeightInTiles < 0)
			{
				colHeightInTiles *= -1;
			}

			float tileMapCenterX = (float)(m_width / 2);
			if (m_width % 2)
				tileMapCenterX += 1;
			else
				tileMapCenterX += 0.5f;

			float tileMapCenterY = (float)(m_height / 2);
			if (m_height % 2)
				tileMapCenterY += 1;
			else
				tileMapCenterY += 0.5f;

			float widthToAdd = colWidthInTiles / 2;
			widthToAdd -= 0.5f;
			float heightToAdd = colHeightInTiles / 2;
			heightToAdd -= 0.5f;


			float colliderCenterX = colliderStart.x + widthToAdd - tileMapCenterX;
			if (colliderEnd.x < colliderStart.x)
			{
				colliderCenterX = colliderEnd.x + widthToAdd - tileMapCenterX;
			}
			float colliderCenterY = colliderStart.y + heightToAdd - tileMapCenterY;
			if (colliderEnd.y < colliderStart.y)
			{
				colliderCenterY = colliderEnd.y + heightToAdd - tileMapCenterY;
			}

			collider->SetActiveOffset(Vector2(colliderCenterX * gridWidthsInATile, -colliderCenterY * gridHeightsInATile));
			collider->SetActiveDimensions(colWidthInTiles * gridWidthsInATile, colHeightInTiles * gridHeightsInATile);
		}
	}

	std::string TileMap::GetSelectedCollisionArea()
	{
		return m_selectedCollisionArea;
	}

	void TileMap::SetSelectedCollisionArea(std::string selectedCollisionArea)
	{
		m_selectedCollisionArea = selectedCollisionArea;
	}

	void TileMap::RecalcCollisionAreaValues()
	{
		for (std::pair<std::string, std::vector<CollisionAreaData>> collisionArea : m_collisionAreas)
		{

		}
	}

	void TileMap::MoveTiles(std::vector<Vector2> tiles, Vector2 moveAmount)
	{
		m_tempMoveTiles.clear();
		
		// Make temp copies first so that no Tile being copied over gets overridden by a Tile before it before it gets copied itself.
		for (Vector2 tile : tiles)
		{
			MoveTile(tile, moveAmount);
		}

		// Copy new, temp tiles into the main TileMap.
		for (std::pair<int, std::map<int, Tile>> tileRow : m_tempMoveTiles)
		{
			for (std::pair<int, Tile> tileCol : tileRow.second)
			{
				int x = tileRow.first;
				int y = tileCol.first;
				Tile tileCopy = tileCol.second;

				if (m_tiles.count(x) > 0 && m_tiles.at(x).count(y) > 0)
				{
					m_tiles.at(x).at(y) = tileCopy;
				}
				else if (m_tiles.count(x) > 0 && m_tiles.at(x).count(y) == 0)
				{
					std::pair<int, Tile> newPair = { y, tileCopy };
					m_tiles.at(x).emplace(newPair);
				}
				else if (m_tiles.count(x) == 0)
				{
					std::pair<int, Tile> newPair = { y, tileCopy };
					std::map<int, Tile> yCoords;
					yCoords.emplace(newPair);
					m_tiles.emplace(x, yCoords);
				}
			}
		}
	}

	void TileMap::MoveTile(Vector2 tileCoords, Vector2 moveAmount)
	{
		int oldX = tileCoords.x;
		int oldY = tileCoords.y;

		if (m_tiles.count(oldX) > 0 && m_tiles.at(oldX).count(oldY) > 0)
		{
			int x = tileCoords.x + moveAmount.x;
			int y = tileCoords.y + moveAmount.y;

			Tile originalTile = m_tiles.at(oldX).at(oldY);
			Tile newTile;
			
			if (originalTile.tileSetName != "")
			{
				newTile.tileSetTexture = GetTileSet(originalTile.tileSetName)->GetTexture()->GetTexture();
			}
			newTile.tileSetIndex = originalTile.tileSetIndex;
			newTile.tileSetName = originalTile.tileSetName;
			newTile.tileCoord = Vector2(x, y);
			newTile.uvStart = originalTile.uvStart;
			newTile.uvEnd = originalTile.uvEnd;

			// Erase old tile data
			if (m_tiles.count(oldX) > 0 && m_tiles.at(oldX).count(oldY) > 0)
			{
				m_tiles.at(oldX).erase(oldY);
			}

			// Copy new data into m_tempMoveTiles to be copied after all tiles are copied
			std::pair<int, Tile> newPair = { y, newTile };
			std::map<int, Tile> yCoords;
			yCoords.emplace(newPair);
			if (m_tempMoveTiles.count(x) > 0)
			{
				m_tempMoveTiles.at(x).emplace(newPair);
			}
			else
			{
				m_tempMoveTiles.emplace(x, yCoords);
			}
		}
	}

	bool TileMap::ContainsCollisionAreaLabel(std::string label)
	{
		return m_collisionAreas.count(label);
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