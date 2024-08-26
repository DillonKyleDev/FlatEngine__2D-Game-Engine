#pragma once
#include "Component.h"
#include "FlatEngine.h"
#include <vector>
#include <string>
#include <map>

namespace FlatEngine
{
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
        void AddTileSet(std::string name);
        void RemoveTileSet(std::string name);
        void SetTileSets(std::vector<std::string> tileSets);
        std::vector<std::string> GetTileSets();
        std::map<int, std::pair<SDL_Texture*, std::pair<Vector2, Vector2>>> GetIndexedTiles();

        void SetTile(int tileMapIndex, TileSet* tileSet, int tileSetIndex);
        void CreateTileMap();

    private:
        int m_width;
        int m_height;
        int m_tileWidth;
        int m_tileHeight;
        // index in TileMap, <TileSet name, index in TileSet> // For loading the data from files
        std::map<int, std::pair<std::string, int>> m_storedTextureUVs;
        // For faster access once data has been collected from files
        std::map<int, std::pair<SDL_Texture*, std::pair<Vector2, Vector2>>> m_indexedTextureUVs;
        std::string m_selectedTileSet;
        std::vector<std::string> m_tileSetNames;  
    };
}

