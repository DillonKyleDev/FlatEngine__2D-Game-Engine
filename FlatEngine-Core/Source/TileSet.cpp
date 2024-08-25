#include "TileSet.h"
#include "FlatEngine.h"

#include <filesystem>



namespace FlatEngine
{
    TileSet::TileSet()
    {
        m_texturePath = "";
        m_texture = Texture();
        m_allTileUVs = std::map<int, std::pair<Vector2, Vector2>>();
        m_tileSetUVs = std::map<int, std::pair<Vector2, Vector2>>();
        m_tileWidth = 16;
        m_tileHeight = 16;
    }

    TileSet::~TileSet()
    {
    }

    std::string TileSet::GetData()
    {
        json jsonData = {
            { "name", m_name },
            { "tileSetPath", m_tileSetPath },
            { "texturePath", m_texturePath },
            { "tileWidth", m_tileWidth },
            { "tileHeight", m_tileHeight },
        };

        std::string data = jsonData.dump();
        // Return dumped json object with required data for saving
        return data;
    }

    void TileSet::SetName(std::string name)
    {
        m_name = name;
    }

    std::string TileSet::GetName()
    {
        return m_name;
    }

    void TileSet::SetTexturePath(std::string texturePath)
    {
        m_texturePath = texturePath;
        m_texture.LoadFromFile(m_texturePath);
        InitializeUVs();
    }

    std::string TileSet::GetTexturePath()
    {
        return m_texturePath;
    }

    void TileSet::SetTileSetPath(std::string tileSetPath)
    {
        m_tileSetPath = tileSetPath;
    }

    std::string TileSet::GetTileSetPath()
    {
        return m_tileSetPath;
    }

    Texture* TileSet::GetTexture()
    {
        return &m_texture;
    }

    std::map<int, std::pair<Vector2, Vector2>> TileSet::GetTileSet()
    {
        return m_tileSetUVs;
    }

    void TileSet::SetTileWidth(int width)
    {
        m_tileWidth = width;
    }

    void TileSet::SetTileHeight(int height)
    {
        m_tileHeight = height;
    }

    int TileSet::GetTileWidth()
    {
        return m_tileWidth;
    }

    int TileSet::GetTileHeight()
    {
        return m_tileHeight;
    }

    void TileSet::SaveCopyOfTexture()
    {
        try
        {
            std::filesystem::copy(m_texturePath, GetDir("tileTextures"), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
            // save the new file path            
            m_texturePath = GetDir("tileTextures") + GetFilenameFromPath(m_texturePath, true);
        }
        catch (std::exception& e)
        {
            LogString("ERROR : Failed to copy texture to saved tile textures");
            LogString(e.what());
        }
    }

    void TileSet::InitializeUVs()
    {
        if (m_tileWidth != 0 && m_tileHeight != 0 && m_texture.GetTexture() != nullptr)
        {
            int textureWidth = m_texture.GetWidth();
            int textureHeight = m_texture.GetHeight();
            int tilesAcross = textureWidth / m_tileWidth;
            int tilesDown = textureHeight / m_tileHeight;

            // if the desired tile dimensions are not aligned perfectly with the texture dimensions, add 1 more tile across or down for the overhang
            if (textureWidth % m_tileWidth != 0)
                tilesAcross += 1;
            if (textureHeight % m_tileHeight != 0)
                tilesDown += 1;

            m_allTileUVs.clear();

            int counter = 0;

            for (int h = 0; h < tilesAcross; h++)
            {
                for (int v = 0; v < tilesDown; v++)
                {
                    Vector2 uvStart = Vector2((float)(h * m_tileWidth), (float)(v * m_tileHeight));
                    Vector2 uvEnd = Vector2((float)(uvStart.x + m_tileWidth), (float)(uvStart.y + m_tileHeight));

                    // Make sure we're not saving a UV position that is outside the bounds of the SDL_Texture
                    if (textureWidth % m_tileWidth != 0 && h == tilesAcross - 1)
                        uvEnd.x -= textureWidth % m_tileWidth;
                    if (textureHeight % m_tileHeight != 0 && v == tilesDown - 1)
                        uvEnd.x -= textureHeight % m_tileHeight;

                    std::pair<Vector2, Vector2> uvs = { uvStart, uvEnd };
                    m_allTileUVs.emplace(counter, uvs);
                }
            }
        }
    }

    void TileSet::ToggleTile(int index)
    {
        if (m_tileSetUVs.count(index) > 0)
            m_tileSetUVs.erase(index);
        else if (m_allTileUVs.count(index) > 0)
            m_tileSetUVs.emplace(index, m_allTileUVs.at(index));
    }
}