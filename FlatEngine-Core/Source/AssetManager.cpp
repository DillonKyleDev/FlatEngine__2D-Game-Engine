#pragma once
#include "AssetManager.h"
#include "FlatEngine.h"
#include "Texture.h"
#include "Vector4.h"
#include "imgui.h"
#include <fstream>

namespace FlatEngine
{
	AssetManager::AssetManager()
	{
		m_directories = std::map<std::string, std::string>();
		m_files = std::map<std::string, std::string>();
		m_colors = std::map<std::string, Vector4>();
		m_textures = std::map<std::string, std::shared_ptr<Texture>>();
		m_errorTexture = Texture();
		m_resourceFailedToLoadImagePath = "";
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::GetDirectoryPaths()
	{
		static std::string directoriesFileLocation = "";

		if (!DoesFileExist("directory_location.lua"))
		{
			BeginImGuiRender();
			ImGui::SetNextWindowPos(Vector2(0, 0));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
			BeginWindow("Select Directories.lua File Location");
			BeginWindowChild("TextPrompt");

			ImGui::Text("Please select the Directories.lua file location within the FlatEngine project directory.");
			ImGui::Text("");
			ImGui::TextWrapped("If you haven't made any changes to the project, it should be located in: \"FlatEngine/assets/engine-assets/scripts/Directories.lua\"");			
			ImGui::Text(directoriesFileLocation.c_str());
			ImGui::Text("Chosen path");

			EndWindowChild();
			EndWindow();
			EndImGuiRender();

			// Get Directories filepath from user
			directoriesFileLocation = OpenLoadFileExplorer();

			if (DoesFileExist(directoriesFileLocation))
			{
				// Save this filepath to a new lua file called directory_location.lua
				std::ofstream outfile;
				std::string filenameWExtention = "directory_location.lua";
				outfile.open(filenameWExtention, std::ios_base::app);
				std::replace(directoriesFileLocation.begin(), directoriesFileLocation.end(), '\\', '/');
				outfile << "-- Location of the Directories.lua file that houses the different build directory files \n\n\n  F_DirectoryPath = \"" + directoriesFileLocation + "\"";
			}
			else
				GetDirectoryPaths(); // Ask again
		}
		else
		{
			// if the file does exist, load the Directories.lua file saved previously inside directory_location.lua
			auto script = F_Lua.safe_script_file("directory_location.lua");						
			directoriesFileLocation = F_Lua["F_DirectoryPath"].get_or<std::string>("");
		}

		// Get the data from the Directories.lua file
		if (directoriesFileLocation != "")
		{
			auto script = F_Lua.safe_script_file(directoriesFileLocation);
			std::optional<sol::table> dirTable = F_Lua["F_DirectoryPaths"];

			// File Paths
			if (dirTable)
			{
				for (const auto& entry : dirTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					if (sKey == "runtime")
						F_RuntimeDirectoriesLuaFilepath = sValue;
					else if (sKey == "editor")
						F_EditorDirectoriesLuaFilepath = sValue;
					else if (sKey == "debug")
						F_DebugDirectoriesLuaFilepath = sValue;
				}
			}
		}
		else
			GetDirectoryPaths(); // Ask again
	}

	void AssetManager::CollectDirectories(DirectoryType dirType)
	{
		m_directories.clear();
		m_files.clear();
		std::string dirPath = "";

		switch (dirType)
		{
		case RuntimeDir:
			dirPath = F_RuntimeDirectoriesLuaFilepath;
			break;
		case EditorDir:
			dirPath = F_EditorDirectoriesLuaFilepath;
			break;
		case DebugDir:
			dirPath = F_DebugDirectoriesLuaFilepath;
			break;
		default:
			break;
		}

		// Load in lua script
		if (DoesFileExist(dirPath))
		{
			auto script = F_Lua.safe_script_file(dirPath);
			std::optional<sol::table> dirTable = F_Lua["F_Dirs"];
			std::optional<sol::table> pathTable = F_Lua["F_Paths"];

			// Directories
			if (dirTable)
			{
				for (const auto& entry : dirTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					m_directories.emplace(sKey, sValue);
				}
			}

			// File Paths
			if (pathTable)
			{
				for (const auto& entry : pathTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					m_files.emplace(sKey, sValue);
				}
			}
		}
	}

	void AssetManager::CollectColors()
	{
		m_colors.clear();

		// Load in lua script
		if (DoesFileExist(GetFilePath("colors")))
		{
			auto script = F_Lua.safe_script_file(GetFilePath("colors"));
			std::optional<sol::table> colorTable = F_Lua["F_Colors"];

			if (colorTable)
			{
				for (const auto& entry : colorTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					Vector4 sValue = value.as<Vector4>();         // cast key as a Vector4

					m_colors.emplace(sKey, sValue);
				}
			}
		}
	}	

	void AssetManager::CollectTextures()
	{
		m_textures.clear();

		// Load in lua script
		if (DoesFileExist(GetFilePath("textures")))
		{
			auto script = F_Lua.safe_script_file(GetFilePath("textures"));
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

	std::string AssetManager::GetDir(std::string dirName)
	{
		if (m_directories.count(dirName))
			return m_directories.at(dirName);
		else
			return "";
	}

	std::string AssetManager::GetFilePath(std::string fileName)
	{
		if (m_files.count(fileName))
			return m_files.at(fileName);
		else
			return "";
	}

	Vector4 AssetManager::GetColor(std::string colorName)
	{
		if (m_colors.count(colorName))
			return m_colors.at(colorName);
		else
			return Vector4(0.08f, 0.08f, 0.10f, 1.0f);
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