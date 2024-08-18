#include "FlatGui.h"
#include "FlatEngine.h"
#include "AssetManager.h"

#include <process.h>
#include <Windows.h>
#include <iostream>


namespace FlatGui
{
	std::map<std::string, bool> FG_fileExplorerLeafTracker = std::map<std::string, bool>();
	std::vector<std::string> FG_clickedExplorerFiles = std::vector<std::string>();

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
		float availableWidth = ImGui::GetContentRegionMax().x;
		float iconWidth = (float)FlatEngine::GetTextureObject("folderFileIcon")->GetWidth();
		float iconHeight = (float)FlatEngine::GetTextureObject("folderFileIcon")->GetHeight();
		float horizontalSpacing = 10;
		float verticalSpacing = 0;
		int maxIconsPerRow = (int)(availableWidth / (iconWidth + horizontalSpacing) - 1);
		int iconsThisRow = 0;

		//FlatEngine::LogString(fs_filepath.string());
		if (std::filesystem::is_directory(fs_filepath))
		{
			for (const auto& entry : std::filesystem::directory_iterator(fs_filepath.string()))
			{				
				if (iconsThisRow == 0)
					ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));
				Vector2 currentPos = ImGui::GetCursorScreenPos();

				RenderFileIcon(entry, Vector2(iconWidth, iconHeight), currentPos);

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

	void RenderFileIcon(std::filesystem::path fs_filepath, Vector2 dimensions, Vector2 currentPos)
	{
		std::string extension = fs_filepath.extension().string();
		std::string icon;
		Vector2 filenamePadding = Vector2(5, 15);

		if (std::filesystem::is_directory(fs_filepath.string()))
			icon = "folderFileIcon";
		else if (extension == ".cpp")
			icon = "cppFileIcon";
		else if (extension == ".h")
			icon = "hFileIcon";
		else if (extension == ".lua")
			icon = "luaFileIcon";
		else if (extension == ".png")
			icon = "pngFileIcon";
		else
			icon = "fileFileIcon";

		SDL_Texture* texture = FlatEngine::GetTexture(icon);

		

		if (FlatEngine::RenderImageButton("##" + icon + fs_filepath.string(), texture, dimensions, 5, FlatEngine::F_transparentColor))
		{

		}
		ImGui::SetCursorScreenPos(Vector2(currentPos.x + 5, currentPos.y + dimensions.y + filenamePadding.y));
		ImGui::SetNextItemWidth(dimensions.x * 0.8f);
		ImGui::TextWrapped(fs_filepath.filename().string().c_str());
	}
}