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
		static Vector2 scrolling = Vector2(0, 0);
		static Vector2 gridStep = Vector2(40, 40);
		static std::string tileSetName = "";
		static bool b_openTileSetModal = false;
		static std::string tileSetTexturePath = "";
		int droppedValue = -1;

		FL::BeginWindow("Tileset Editor");

		FL::BeginResizeWindowChild("Toolbar");

		ImGui::Text("Toolbar");

		if (FL::RenderButton("New TileSet"))
			b_openTileSetModal = true;

		ImGui::Text("Select mapping context to edit:");
		ImGui::EndChild();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

		if (FL::F_TileSets.size() > 0)
		{
			TileSet* currentTileSet = FL::GetTileSet(FL::F_selectedTileSetToEdit);

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
				if (FL::DropInputCanOpenFiles("##TileSetSource", "TileSet source", tileSetTexturePath, FL::F_fileExplorerTarget, droppedValue, tileSetTexturePath, "Drop images here from the File Explorer"))
				{
					// If drop target, get the selected file path
					if (droppedValue != -1 && FL::F_selectedFiles.size() >= droppedValue)
						tileSetTexturePath = FL::F_selectedFiles[droppedValue - 1];

					currentTileSet->SetTexturePath(tileSetTexturePath);
					FL::SaveTileSet(*currentTileSet);
				}
			}
		}


		if (FL::RenderInputModal("Create New TileSet", "Enter a name for the TileSet", tileSetName, b_openTileSetModal))
		{
			FL::CreateNewTileSetFile(tileSetName, FL::GetDir("tileSets"));
		}

		FL::EndWindowChild();



		// Vertical divider between grid space and toolbar area
		ImGui::SameLine(0,0);





		centerPoint = ImGui::GetCursorScreenPos();
		Vector2 gridStart = ImGui::GetCursorScreenPos();
		Vector2 gridEnd = Vector2(gridStart.x + ImGui::GetContentRegionAvail().x, gridStart.y + ImGui::GetContentRegionAvail().y);



		FL::BeginWindowChild("Gridspace");
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();

		FlatGui::RenderGridView(centerPoint, scrolling, false, gridStart, gridEnd, canvas_sz, gridStep, Vector2(0, 0));

		//if (FL::F_EditingTileSet != nullptr)
		//{
		//	Texture* texture = FL::F_EditingTileSet->GetTexture();
		//	Vector2 tileSize = Vector2(FL::F_EditingTileSet->GetTileWidth(), FL::F_EditingTileSet->GetTileHeight());
		//	SDL_Texture* tileTexture = texture->GetTexture();
		//	float textureWidth = (float)texture->GetWidth();
		//	float textureHeight = (float)texture->GetHeight();
		//	Vector2 pivotOffset = Vector2(0, 0);
		//	Vector2 spriteOffset = Vector2(tileSize.x / -2, tileSize.y / -2);
		//	bool _spriteScalesWithZoom = true;
		//	int renderOrder = 1;
		//	Vector4 tintColor = FL::GetColor("white");

		//	FL::AddImageToDrawList(tileTexture, Vector2(0, 0), centerPoint, textureWidth, textureHeight, Vector2(0, 0), Vector2(1, 1), true, gridStep.x, drawList);

		//	//for (std::pair<int, std::pair<Vector2, Vector2>> tile : FL::F_EditingTileSet->GetTileSet())
		//	//	RenderTileSetEditorTile(tile, texture, tileSize, scrolling, gridStart, canvas_sz, gridStep.x, drawList);
		//}


		FL::EndWindowChild();


		FL::EndWindow();
	}


	//void RenderTileSetEditorTile(std::pair<int, std::pair<Vector2, Vector2>> tile, Texture *texture, Vector2 tileSize, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_sz, float step, ImDrawList* draw_list)
	//{
	//	int index = tile.first;
	//	Vector2 uvStart = tile.second.first;
	//	Vector2 uvEnd = tile.second.second;

	//	SDL_Texture* tileTexture = texture->GetTexture();
	//	float spriteTextureWidth = (float)texture->GetWidth();
	//	float spriteTextureHeight = (float)texture->GetHeight();
	//	Vector2 pivotOffset = Vector2(0,0);
	//	Vector2 spriteOffset = Vector2(tileSize.x / -2, tileSize.y / -2);
	//	bool _spriteScalesWithZoom = true;
	//	int renderOrder = 1;
	//	Vector4 tintColor = FL::GetColor("white");
	//	std::string invisibleButtonID = "##TileSetTileButton" + std::to_string(index);

	//			// Get Input and Output
	//			ImGuiIO& inputOutput = ImGui::GetIO();

	//			Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x - canvas_p0.x + (position.x * step) - ((pivotOffset.x * FL::F_spriteScaleMultiplier * step) * scale.x * spriteScale.x), FG_sceneViewCenter.y - canvas_p0.y - (position.y * step - 20) - ((pivotOffset.y * FL::F_spriteScaleMultiplier * step) * scale.y * spriteScale.y));
	//			ImGui::SetCursorPos(positionOnScreen);
	//			//// This will catch our interactions  - 4096 for overlap or keyword if it works
	//			ImGui::SetNextItemAllowOverlap();
	//			ImGui::InvisibleButton(invisibleButtonID.c_str(), Vector2(spriteTextureWidth * FL::F_spriteScaleMultiplier * step * scale.x * spriteScale.x, spriteTextureHeight * FL::F_spriteScaleMultiplier * step * scale.y * spriteScale.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	//			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	//			const bool is_active = ImGui::IsItemActive();   // Held
	//			const bool is_clicked = ImGui::IsItemClicked();

	//			if (is_clicked)
	//				SetFocusedGameObjectID(sprite->GetParentID());

	//			// Show cursor position in scene view when pressing Alt
	//			if (is_hovered && inputOutput.KeyAlt)
	//				RenderSceneViewTooltip();

	//			// Add the same behavior as the sceneview grid so pan and zoom behaviors are not disabled when view entirely obstructed by sprite
	//			////////////////////////
	//			if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
	//			{
	//				FG_sceneViewScrolling.x += inputOutput.MouseDelta.x;
	//				FG_sceneViewScrolling.y += inputOutput.MouseDelta.y;
	//			}
	//			// Get scroll amount for changing zoom level of scene view
	//			Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
	//			float scrollInput = inputOutput.MouseWheel;
	//			float weight = 0.01f;
	//			float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
	//			float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
	//			bool _weightedScroll = true;
	//			float zoomSpeed = 0.1f;
	//			float zoomMultiplier = 10;
	//			float finalZoomSpeed = zoomSpeed;

	//			if (ImGui::GetIO().KeyCtrl)
	//				finalZoomSpeed *= zoomMultiplier;

	//			// Change scrolling offset based on mouse position and weight
	//			if (is_hovered)
	//			{
	//				if (scrollInput > 0)
	//				{
	//					if (_weightedScroll)
	//					{
	//						FG_sceneViewScrolling.x -= trunc(signedMousePosX * weight);
	//						FG_sceneViewScrolling.y -= trunc(signedMousePosY * weight);
	//					}
	//					FG_sceneViewGridStep.x += finalZoomSpeed;
	//					FG_sceneViewGridStep.y += finalZoomSpeed;
	//				}
	//				else if (scrollInput < 0 && FG_sceneViewGridStep.x > 2 && FG_sceneViewGridStep.y > 2)
	//				{
	//					if (_weightedScroll)
	//					{
	//						FG_sceneViewScrolling.x += trunc(signedMousePosX * weight);
	//						FG_sceneViewScrolling.y += trunc(signedMousePosY * weight);
	//					}
	//					FG_sceneViewGridStep.x -= finalZoomSpeed;
	//					FG_sceneViewGridStep.y -= finalZoomSpeed;
	//				}
	//			}
	//			//////////////////

	//			// Change the draw channel for the scene object
	//			if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
	//				drawSplitter->SetCurrentChannel(draw_list, renderOrder);
	//			else
	//				drawSplitter->SetCurrentChannel(draw_list, 0);

	//			// Draw the texture
	//			FL::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, pivotOffset, Vector2(transformScale.x * spriteScale.x, transformScale.y * spriteScale.y), _spriteScalesWithZoom, step, draw_list, rotation, ImGui::GetColorU32(tintColor));
	//		}
	//	}
	//}
}