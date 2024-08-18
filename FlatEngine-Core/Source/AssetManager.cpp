#pragma once
#include "AssetManager.h"
#include "FlatEngine.h"
#include "Texture.h"
#include "Vector4.h"

namespace FlatEngine
{
	AssetManager::AssetManager()
	{
		m_colors = std::map<std::string, Vector4>();		
		m_textures = std::map<std::string, std::shared_ptr<Texture>>();
		m_errorTexture = Texture();
		m_resourceFailedToLoadImagePath = "";	
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::CollectColors()
	{
		// Load in lua script
		if (DoesFileExist("../engine-assets/scripts/Colors.lua"))
		{
			auto script = F_Lua.safe_script_file("../engine-assets/scripts/Colors.lua");
			std::optional<sol::table> colorTable = F_Lua["F_Colors"];

			if (colorTable)
			{
				for (const auto& entry : colorTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					Vector4 sValue = value.as<Vector4>(); // cast key as a Vector4

					m_colors.emplace(sKey, sValue);
				}
			}
		}
	}	

	void AssetManager::CollectTextures()
	{
		m_textures.clear();

		// Load in lua script
		if (DoesFileExist("../engine-assets/scripts/Textures.lua"))
		{
			auto script = F_Lua.safe_script_file("../engine-assets/scripts/Textures.lua");
			sol::object errPath = F_Lua["F_ResourceFailedToLoadImagePath"];
			m_resourceFailedToLoadImagePath = errPath.as<std::string>();

			std::optional<sol::table> textureTable = F_Lua["F_Textures"];
			if (textureTable)
			{
				for (const auto& entry : textureTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();					
					m_textures.emplace(sKey, newTexture);
					m_textures.at(sKey)->LoadFromFile(sValue);
				}
			}
		}

		// Load error texture
		if (m_resourceFailedToLoadImagePath != "")
			m_errorTexture.LoadFromFile(m_resourceFailedToLoadImagePath);
	}

	Vector4 AssetManager::GetColor(std::string colorName)
	{
		if (m_colors.count(colorName))
			return m_colors.at(colorName);
		else
			return Vector4(0.0f, 0.0f, 1.0f, 1.0f); // Return red
	}

	Uint32 AssetManager::GetColor32(std::string colorName)
	{
		if (m_colors.count(colorName))
			return ImGui::GetColorU32(m_colors.at(colorName));
		else
			return ImGui::GetColorU32(Vector4(0.0f, 0.0f, 1.0f, 1.0f)); // Return red
	}

	std::shared_ptr<Texture> AssetManager::GetTextureObject(std::string textureName)
	{
		if (m_textures.count(textureName))
			return m_textures.at(textureName);
		else
			return nullptr; // Return "failed to load texture" texture
	}

	SDL_Texture* AssetManager::GetTexture(std::string textureName)
	{
		if (m_textures.count(textureName))
			return m_textures.at(textureName)->GetTexture();
		else
			return nullptr; // Return "failed to load texture" texture
	}
}