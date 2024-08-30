#pragma once
#include "Component.h"
#include "FlatEngine.h"
#include <vector>
#include <string>
#include <map>

namespace FlatEngine
{
    class BoxCollider;

    struct Tile {
        Vector2 tileCoord;
        std::string tileSetName;
        SDL_Texture* tileSetTexture;
        int tileSetIndex;
        Vector2 uvStart;
        Vector2 uvEnd;
    };

    class TileMap : public Component
    {
    public:
        TileMap(long myID, long parentID);
        TileMap(TileMap* toCopy, long newParentID, long myID);
        ~TileMap();

        std::string GetData();
        int GetWidth();
        int GetHeight();
        int GetTileWidth();
        int GetTileHeight();
        void SetTileWidth(int width);
        void SetTileHeight(int height);
        void SetWidth(int width);
        void SetHeight(int height);
        std::string GetSelectedTileSet();
        void SetSelectedTileSet(std::string tileSet);
        std::map<std::string, BoxCollider> &GetBoxCollisionAreas();
        void AddBoxCollisionArea(std::string label, BoxCollider collisionArea);
        void AddTileSet(std::string name);
        void RemoveTileSet(std::string name);
        void SetTileSets(std::vector<std::string> tileSets);
        std::vector<std::string> GetTileSets();
        std::map<int, std::map<int, Tile>> GetTiles();
        void SetTile(Vector2 tileMapCoords, TileSet* tileSet, int tileSetIndex);

    private:
        int m_width;
        int m_height;
        int m_tileWidth;
        int m_tileHeight;
        std::map<int, std::map<int, Tile>> m_tiles;
        std::string m_selectedTileSet;
        std::vector<std::string> m_tileSetNames;  
        std::map<std::string, BoxCollider> m_boxCollisionAreas;
    };
}

