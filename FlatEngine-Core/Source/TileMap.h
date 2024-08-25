#pragma once
#include "Component.h"
#include "FlatEngine.h"


namespace FlatEngine
{
    class TileMap : public Component
    {
    public:
        TileMap(long myID, long parentID);
        TileMap(TileMap* toCopy, long newParentID, long myID);
        ~TileMap();

        std::string GetData();



    private:
        std::string m_name;
    };
}

