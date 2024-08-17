#include "FlatGui.h"
#include "FlatEngine.h"
#include "AssetManager.h"

#include <process.h>
#include <Windows.h>
#include <iostream>


namespace FlatGui
{
	std::map<std::string, bool> FG_fileExplorerLeafTracker = std::map<std::string, bool>();

	void RenderFileExplorer()
	{		
		std::string rootDirPath = "../";
		std::filesystem::path rootPath(rootDirPath);
		static std::string filepath_clicked = rootDirPath;
		std::error_code err;
		bool b_isDirectory = std::filesystem::is_directory(rootPath, err);

		FlatEngine::BeginWindow("File Explorer");
		// {
			FlatEngine::BeginResizeWindowChild("Directories Panel", FlatEngine::F_logBgColor);
			// {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_innerWindowColor);
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
				FlatEngine::PushMenuStyles();

				if (ImGui::BeginTable("##AnimationProperties", 1, FlatEngine::F_tableFlags))
				{
					ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x);

					if (b_isDirectory)
						RenderDirNodes(rootDirPath, filepath_clicked);

					ImGui::EndTable();
				}
				FlatEngine::PopMenuStyles();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			// }
			FlatEngine::EndWindowChild();




			// Vertical Division between hierarchy and view panel ------------------------------------------------
			ImGui::SameLine(0, 5);



			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(100,100));
			FlatEngine::BeginWindowChild("Files Panel", FlatEngine::F_explorerFilesBg);
			ImGui::PopStyleVar();
			// {				
				RenderDirItems(filepath_clicked);				
			// }
			FlatEngine::EndWindowChild();

		// }
		FlatEngine::EndWindow();
	}

	void RenderDirNodes(std::string dir, std::string& filepath_clicked)
	{
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			bool b_isDirectory = std::filesystem::is_directory(entry.path().string());
			if (b_isDirectory)
				RenderDirNode(entry.path(), filepath_clicked);
		}
	}

	void RenderDirNode(std::filesystem::path fs_filepath, std::string& filepath_clicked)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGuiTreeNodeFlags nodeFlags;
		std::string treeID = fs_filepath.string() + "_node";				
		bool b_nodeOpen = false;		
		std::error_code err;
		bool b_isDirectory = std::filesystem::is_directory(fs_filepath, err);
		//bool b_isFile = std::filesystem::is_regular_file(fs_filepath, err);

		// If this node is selected, use the node_selected to highlight it
		if (filepath_clicked == fs_filepath.string())
		{
			if (b_isDirectory)
				nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		}
		// Not selected
		else
		{
			if (b_isDirectory)
				nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			else
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		}

		// if it is a directory
		if (b_isDirectory)
		{
			if (FG_fileExplorerLeafTracker.count(treeID))
				ImGui::SetNextItemOpen(FG_fileExplorerLeafTracker.at(treeID));
			else
				ImGui::SetNextItemOpen(false);

			b_nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, fs_filepath.filename().string().c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				filepath_clicked = fs_filepath.string();

			if (FG_fileExplorerLeafTracker.count(treeID))
				FG_fileExplorerLeafTracker.at(treeID) = b_nodeOpen;
			else
				FG_fileExplorerLeafTracker.emplace(treeID, b_nodeOpen);

			if (b_nodeOpen)
			{
				RenderDirNodes(fs_filepath.string(), filepath_clicked);
				ImGui::TreePop();
			}
		}
		// render a leaf
		else
		{			
			ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, fs_filepath.filename().string().c_str());
			if (ImGui::IsItemClicked())
				filepath_clicked = fs_filepath.string();
		}
	}

	void RenderDirItems(std::filesystem::path fs_filepath)
	{
		static std::shared_ptr<Texture> cppFileIcon = std::make_shared<Texture>();
		cppFileIcon->LoadFromFile(FlatEngine::F_AssetManager->GetPath("cppFileIcon"));
		static std::shared_ptr<Texture> hFileIcon = std::make_shared<Texture>();
		hFileIcon->LoadFromFile(FlatEngine::F_AssetManager->GetPath("hFileIcon"));
		static std::shared_ptr<Texture> luaFileIcon = std::make_shared<Texture>();
		luaFileIcon->LoadFromFile(FlatEngine::F_AssetManager->GetPath("luaFileIcon"));
		static std::shared_ptr<Texture> pngFileIcon = std::make_shared<Texture>();
		pngFileIcon->LoadFromFile(FlatEngine::F_AssetManager->GetPath("pngFileIcon"));
		static std::shared_ptr<Texture> folderFileIcon = std::make_shared<Texture>();
		folderFileIcon->LoadFromFile(FlatEngine::F_AssetManager->GetPath("folderFileIcon"));

		//FlatEngine::F_AssetManager->GetPath("cppFileIcon")
		float availableWidth = ImGui::GetContentRegionMax().x;
		float iconWidth = (float)FlatEngine::F_cppFileIcon.GetWidth();
		float iconHeight = (float)FlatEngine::F_cppFileIcon.GetHeight();
		float horizontalSpacing = 30;
		float verticalSpacing = 15;
		Vector2 filenamePadding = Vector2(5, 15);
		int maxIconsPerRow = (int)(availableWidth / (iconWidth + horizontalSpacing) - 1);
		int iconsThisRow = 0;

		//FlatEngine::LogString(fs_filepath.string());
		if (std::filesystem::is_directory(fs_filepath))
		{
			for (const auto& entry : std::filesystem::directory_iterator(fs_filepath.string()))
			{				
				//FlatEngine::LogInt(FlatEngine::F_AssetManager->GetTexture("folderFileIcon").GetWidth());
				std::string extension = entry.path().extension().string();	

				if (extension == ".cpp")
				{
					if (iconsThisRow == 0)
						ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					Vector2 currentPos = ImGui::GetCursorScreenPos();
					Texture failedText = Texture();
					failedText.LoadFromFile("Source/assets/images/icons/cpp.png");
					//FlatEngine::F_AssetManager->m_errorTexture = &failedText;
					//FlatEngine::F_AssetManager->GetTexture("cppFileIcon")->LoadFromFile("Source/assets/images/icons/cpp.png");
					if (FlatEngine::RenderImageButton("##cppFileIcon-" + entry.path().string(), cppFileIcon->GetTexture(), Vector2(iconWidth, iconHeight), 0, FlatEngine::F_transparentColor))
					{
						FlatEngine::LogString(entry.path().filename().string());
					}
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + iconHeight + filenamePadding.y));
					ImGui::SetNextItemWidth(iconWidth * 0.8f);
					ImGui::TextWrapped(entry.path().filename().string().c_str());

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconWidth + horizontalSpacing, currentPos.y));
						iconsThisRow++;
					}
					else
						iconsThisRow = 0;
				}
				else if (extension == ".h")
				{
					if (iconsThisRow == 0)
						ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					Vector2 currentPos = ImGui::GetCursorScreenPos();					
					if (FlatEngine::RenderImageButton("##hFileIcon-" + entry.path().string(), hFileIcon->GetTexture(), Vector2(iconWidth, iconHeight), 0, FlatEngine::F_transparentColor))
					{
						FlatEngine::LogString(entry.path().filename().string());
					}
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + iconHeight + filenamePadding.y));
					ImGui::SetNextItemWidth(iconWidth * 0.8f);
					ImGui::TextWrapped(entry.path().filename().string().c_str());

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconWidth + horizontalSpacing, currentPos.y));
						iconsThisRow++;
					}
					else
						iconsThisRow = 0;
				}
				else if (extension == ".lua")
				{
					if (iconsThisRow == 0)
						ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					Vector2 currentPos = ImGui::GetCursorScreenPos();					
					if (FlatEngine::RenderImageButton("##LuaFileIcon-" + entry.path().string(), luaFileIcon->GetTexture(), Vector2(iconWidth, iconHeight), 0, FlatEngine::F_transparentColor))
					{
						FlatEngine::LogString(entry.path().filename().string());
					}
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + iconHeight + filenamePadding.y));
					ImGui::SetNextItemWidth(iconWidth * 0.8f);
					ImGui::TextWrapped(entry.path().filename().string().c_str());

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconWidth + horizontalSpacing, currentPos.y));
						iconsThisRow++;
					}
					else
						iconsThisRow = 0;
				}
				else if (extension == ".png")
				{
				/*	static std::shared_ptr<Texture> pngThumbnail = std::make_shared<Texture>();
					pngThumbnail->LoadFromFile(entry.path().filename().string());*/

					if (iconsThisRow == 0)
						ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					Vector2 currentPos = ImGui::GetCursorScreenPos();					
					if (FlatEngine::RenderImageButton("##pngFileIcon-" + entry.path().string(), pngFileIcon->GetTexture(), Vector2(iconWidth, iconHeight), 0, FlatEngine::F_transparentColor))
					{
						FlatEngine::LogString(entry.path().filename().string());
					}
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + iconHeight + filenamePadding.y));
					ImGui::SetNextItemWidth(iconWidth * 0.8f);
					ImGui::TextWrapped(entry.path().filename().string().c_str());

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconWidth + horizontalSpacing, currentPos.y));
						iconsThisRow++;
					}
					else
						iconsThisRow = 0;
				}
				else if (std::filesystem::is_directory(entry.path().string()))
				{
					if (iconsThisRow == 0)
						ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
					Vector2 currentPos = ImGui::GetCursorScreenPos();
					if (FlatEngine::RenderImageButton("##folderFileIcon-" + entry.path().string(), folderFileIcon->GetTexture(), Vector2(iconWidth, iconHeight), 0, FlatEngine::F_transparentColor))
					{
						FlatEngine::LogString(entry.path().filename().string());
					}
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + iconHeight + filenamePadding.y));
					ImGui::SetNextItemWidth(iconWidth * 0.8f);
					ImGui::TextWrapped(entry.path().filename().string().c_str());

					if (iconsThisRow != maxIconsPerRow)
					{
						ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconWidth + horizontalSpacing, currentPos.y));
						iconsThisRow++;
					}
					else
						iconsThisRow = 0;
				}
			}
		}
	}
}