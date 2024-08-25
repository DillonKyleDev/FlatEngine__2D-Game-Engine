#include "TileMap.h"


namespace FlatEngine
{
	FlatEngine::TileMap::TileMap(long myID, long parentID)
	{
		SetType(ComponentTypes::T_TileMap);
		SetID(myID);
		SetParentID(parentID);
	}

	TileMap::TileMap(TileMap* toCopy, long myID, long newParentID)
	{
		SetType(ComponentTypes::T_TileMap);
		SetID(myID);
		SetParentID(newParentID);
	}

	TileMap::~TileMap()
	{
	}

	std::string TileMap::GetData()
	{
		return std::string();
	}
}
