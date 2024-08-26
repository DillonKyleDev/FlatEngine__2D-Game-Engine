#include "FlatEngine.h"
#include "FlatGui.h"
#include "TileSet.h"
#include "Vector2.h"

#include <string>
#include <map>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"

namespace FL = FlatEngine;

using TileSet = FL::TileSet;


namespace FlatGui
{
	void RenderTileSetEditor()
	{
		Vector2 centerPoint;
		static Vector2 scrolling = Vector2(0,0);
		static Vector2 gridStep = Vector2(50, 50);
		static std::string tileSetName = "";
		static bool b_openTileSetModal = false;
		static std::string tileSetTexturePath = "";
		int droppedValue = -1;
		TileSet* currentTileSet = nullptr;

		FL::BeginWindow("Tileset Editor");

		FL::BeginResizeWindowChild("Toolbar");

		ImGui::Text("Toolbar");

		if (FL::RenderButton("New TileSet"))
			b_openTileSetModal = true;

		ImGui::Text("Select mapping context to edit:");

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

		if (FL::F_TileSets.size() > 0)
		{
			currentTileSet = FL::GetTileSet(FL::F_selectedTileSetToEdit);

			FL::PushComboStyles();
			//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 184);
			if (ImGui::BeginCombo("##tileSets", FL::F_selectedTileSetToEdit.c_str()))
			{
				for (int i = 0; i < FL::F_TileSets.size(); i++)
				{

					bool is_selected = (FL::F_TileSets.at(i).GetName() == FL::F_selectedTileSetToEdit);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("outerWindow"));
					if (ImGui::Selectable(FL::F_TileSets.at(i).GetName().c_str(), is_selected))
						FL::F_selectedTileSetToEdit = FL::F_TileSets.at(i).GetName();
					if (is_selected)
						ImGui::SetItemDefaultFocus();
					ImGui::PopStyleColor();
				}
				ImGui::EndCombo();
			}
			FL::PopComboStyles();

			if (currentTileSet != nullptr)
			{
				tileSetTexturePath = currentTileSet->GetTexturePath();

				if (FL::DropInputCanOpenFiles("##TileSetSource", "TileSet source", tileSetTexturePath, FL::F_fileExplorerTarget, droppedValue, tileSetTexturePath, "Drop images here from the File Explorer"))
				{
					// If drop target, get the selected file path
					if (droppedValue != -1 && FL::F_selectedFiles.size() >= droppedValue)
						tileSetTexturePath = FL::F_selectedFiles[droppedValue - 1];

					currentTileSet->SetTexturePath(tileSetTexturePath);
					FL::SaveTileSet(*currentTileSet);
				}

				for (int tileIndex : currentTileSet->GetTileSetIndices())
				{
					ImGui::Text(std::to_string(tileIndex).c_str());
				}
			}
		}


		// New TileSet Modal Popup
		if (FL::RenderInputModal("Create New TileSet", "Enter a name for the TileSet", tileSetName, b_openTileSetModal))
		{
			FL::CreateNewTileSetFile(tileSetName, FL::GetDir("tileSets"));
		}

		FL::EndWindowChild();



		// Vertical divider between grid space and toolbar area
		ImGui::SameLine(0,0);






		centerPoint = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		Vector2 gridStart = ImGui::GetCursorScreenPos();
		Vector2 gridEnd = Vector2(gridStart.x + ImGui::GetContentRegionAvail().x, gridStart.y + ImGui::GetContentRegionAvail().y);


		FL::BeginWindowChild("Gridspace", FL::GetColor("outerWindow"), 16 | 8); // No Scrolling flags 
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		Vector2 currentPos = ImGui::GetCursorScreenPos();

		// This will catch our interactions
		ImGui::SetCursorScreenPos(currentPos);
		ImGui::SetNextItemAllowOverlap();
		AddSceneViewMouseControls("#TileSetEditorGrid", gridStart, canvas_sz, scrolling, centerPoint, gridStep);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		FlatGui::RenderGridView(centerPoint, scrolling, false, gridStart, gridEnd, canvas_sz, gridStep, Vector2(0, 0), false);

		if (currentTileSet != nullptr)
		{
			std::shared_ptr<Texture> texture = currentTileSet->GetTexture();
			Vector2 tileSize = Vector2((float)currentTileSet->GetTileWidth(), (float)currentTileSet->GetTileHeight());
			SDL_Texture* tileTexture = texture->GetTexture();
			float textureWidth = (float)texture->GetWidth();
			float textureHeight = (float)texture->GetHeight();
			Vector2 pivotOffset = Vector2(textureWidth / 2, 0);
			Vector2 spriteOffset = Vector2(tileSize.x / 2, tileSize.y / 2);
			bool _spriteScalesWithZoom = true;
			int renderOrder = 1;
			Vector4 tintColor = FL::GetColor("white");
			
			FL::AddImageToDrawList(tileTexture, Vector2(0,0), centerPoint, textureWidth, textureHeight, Vector2(0, 0), Vector2(1, 1), true, gridStep.x, ImGui::GetWindowDrawList());
			for (std::pair<int, std::pair<Vector2, Vector2>> tile : currentTileSet->GetTileSet())
				RenderTileSetEditorTile(tile, tileSize, scrolling, gridStart, canvas_sz, gridStep, currentTileSet);
		}

		FL::EndWindowChild();


		FL::EndWindow();
	}


	void RenderTileSetEditorTile(std::pair<int, std::pair<Vector2, Vector2>> tile, Vector2 tileSize, Vector2 &scrolling, Vector2 gridStart, Vector2 canvas_sz, Vector2 &step, TileSet* tileSet)
	{
		int index = tile.first;
		bool b_selected = false;

		std::vector<int> selectedIndices = tileSet->GetTileSetIndices();
		for (int i = 0; i < selectedIndices.size(); i++)
			if (index == selectedIndices[i])
				b_selected = true;

		Vector2 uvStart = tile.second.first;
		Vector2 uvEnd = tile.second.second;
		float tileWidthInPx = step.x * (tileSize.x / FL::F_pixelsPerGridSpace);
		float hIndex = uvStart.x / tileSize.x;
		float tileHeightInPx = step.y * (tileSize.y / FL::F_pixelsPerGridSpace);
		float vIndex = uvStart.y / tileSize.y;

		float tileStartX = (hIndex * tileWidthInPx) + gridStart.x + scrolling.x;
		float tileStartY = (vIndex * tileHeightInPx) + gridStart.y + scrolling.y;
		Vector2 tileStart = Vector2(tileStartX, tileStartY);
		Vector2 tileEnd = Vector2(tileStartX + tileWidthInPx, tileStartY + tileHeightInPx);

		std::string invisibleButtonID = "##TileSetEditorTileToggle-" + std::to_string(index);
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();
		bool _weightedScroll = true;

		// Mouse controls
		AddSceneViewMouseControls(invisibleButtonID, tileStart, Vector2(tileWidthInPx, tileHeightInPx), scrolling, gridStart, step, FL::GetColor32("tileSetTileBorder"));
		const bool is_hovered = ImGui::IsItemHovered();
		const bool is_active = ImGui::IsItemActive(); 
		const bool is_clicked = ImGui::IsItemClicked();

		if (is_hovered)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
		}
		else if (b_selected)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetSelectedTile"));
		}
		if (is_clicked)
		{
			tileSet->ToggleTile(index);		
			FL::SaveTileSet(*tileSet);
		}
		if (is_active)
			ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoldingTile"));
	}
}