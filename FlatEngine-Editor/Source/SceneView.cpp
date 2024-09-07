#include "FlatEngine.h"
#include "FlatGui.h"
#include "Transform.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"
#include "BoxCollider.h"
#include "imgui_internal.h"

using Transform = FlatEngine::Transform;

namespace FlatGui 
{
	// Scene view default values
	// The multiplier for sceneViewGridStep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	Vector2 FG_sceneViewGridStep = Vector2(50,50);
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool FG_b_firstSceneRenderPass = true;
	bool FG_b_sceneHasBeenSet = false;
	Vector2 FG_sceneViewScrolling = Vector2(0,0);
	Vector2 FG_sceneViewCenter = Vector2(0, 0);
	bool FG_b_sceneViewLockedOnObject = false;
	long FG_sceneViewLockedObjectID = -1;

	bool F_b_inputFocused = false;

	void Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));

		FlatEngine::PushWindowStyles();
		ImGui::Begin("Scene View", 0, 16 | 8); // Window flags 	ImGuiWindowFlags_NoScrollWithMouse
		FlatEngine::PopWindowStyles();

		FL::F_b_sceneViewFocused = ImGui::IsWindowFocused();


		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();

		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set initial viewport dimensions for rendering scene view grid and objects
		if (!FG_b_firstSceneRenderPass)
		{
			if (!FG_b_sceneHasBeenSet)
			{
				SCENE_VIEWPORT_WIDTH = canvas_sz.x;
				SCENE_VIEWPORT_HEIGHT = canvas_sz.y;
				FG_b_sceneHasBeenSet = true;
			}
		}
		FG_b_firstSceneRenderPass = false;

		// For calculating scrolling mouse position and what vector to zoom to
		DYNAMIC_VIEWPORT_WIDTH = trunc(canvas_p1.x - canvas_p0.x);
		DYNAMIC_VIEWPORT_HEIGHT = trunc(canvas_p1.y - canvas_p0.y);

		// Get Input and Output
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();
		Vector2 centerOffset = Vector2(SCENE_VIEWPORT_WIDTH / 2, SCENE_VIEWPORT_HEIGHT / 2);
		bool _weightedScroll = true;

		// This will catch our interactions
		ImGui::SetCursorScreenPos(currentPos);
		ImGui::SetNextItemAllowOverlap();
		ImGui::InvisibleButton("SceneViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);		
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held


		RenderGridView(FG_sceneViewCenter, FG_sceneViewScrolling, _weightedScroll, canvas_p0, canvas_p1, canvas_sz, FG_sceneViewGridStep, centerOffset);

		int droppedValue = -1;
		// Drop Target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::F_fileExplorerTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				droppedValue = *(const int*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		// Create a GameObject from a file in the Explorer by dragging it into the Scene View space
		if (droppedValue != -1 && FL::F_selectedFiles.size() >= droppedValue)
		{
			ImGuiIO& inputOutput = ImGui::GetIO();
			Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - FG_sceneViewCenter.x) / FG_sceneViewGridStep.x, -(inputOutput.MousePos.y - FG_sceneViewCenter.y) / FG_sceneViewGridStep.y);
			std::string filePath = FL::F_selectedFiles[droppedValue - 1];
			FL::LogString(filePath);
			
			FL::CreateAssetUsingFilePath(filePath, mousePosInGrid);
		}

		// Get currently loaded scene objects
		Scene* loadedScene = FlatEngine::F_SceneManager.GetLoadedScene();
		std::map<long, GameObject> sceneObjects;

		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::map<long, GameObject>();

		// Temporary fix.. Add support for map of long,GameObject instead of vector of GameObject
		std::vector<GameObject> viewObjects;
		for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
		{
			viewObjects.push_back(iter->second);
			iter++;
		}


		RenderViewObjects(viewObjects, FG_sceneViewCenter, canvas_p0, canvas_sz, FG_sceneViewGridStep.x);		


		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			// This does not seem to work properly when resizing the window
			// inputOutput.MousePos and MouseDelta give incorrect values after upon dragging the mouse
			FG_sceneViewScrolling.x += inputOutput.MouseDelta.x;
			FG_sceneViewScrolling.y += inputOutput.MouseDelta.y;
		}

		// Show cursor position in scene view when pressing Alt
		if (is_hovered && inputOutput.KeyAlt)
			RenderSceneViewTooltip();

		// For centering on focused GameObject
		GameObject* lockedObject = FL::GetObjectById(FG_sceneViewLockedObjectID);
		if (FG_b_sceneViewLockedOnObject && lockedObject != nullptr)
		{
			Transform* transform = lockedObject->GetTransform();
			Vector2 position = transform->GetTruePosition();
			FG_sceneViewScrolling = Vector2(position.x * -FG_sceneViewGridStep.x + (ImGui::GetWindowWidth() / 2), position.y * FG_sceneViewGridStep.y + (ImGui::GetWindowHeight() / 2));
		}

		Vector2 adjustedScrolling = Vector2(FG_sceneViewScrolling.x + centerOffset.x, FG_sceneViewScrolling.y + centerOffset.y);

		// Get scroll amount for changing zoom level of scene view
		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.01f;
		float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
		float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
		float zoomSpeed = 0.1f;
		float zoomMultiplier = 10;
		float finalZoomSpeed = zoomSpeed;
		
		if (inputOutput.KeyCtrl)
			finalZoomSpeed *= zoomMultiplier;

		// Change scrolling offset based on mouse position and weight
		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				if (_weightedScroll)
				{
					FG_sceneViewScrolling.x -= trunc(signedMousePosX * weight);
					FG_sceneViewScrolling.y -= trunc(signedMousePosY * weight);
				}
				FG_sceneViewGridStep.x += finalZoomSpeed;
				FG_sceneViewGridStep.y += finalZoomSpeed;
			}
			else if (scrollInput < 0 && FG_sceneViewGridStep.x > 2 && FG_sceneViewGridStep.y > 2)
			{
				if (_weightedScroll)
				{
					FG_sceneViewScrolling.x += trunc(signedMousePosX * weight);
					FG_sceneViewScrolling.y += trunc(signedMousePosY * weight);
				}
				FG_sceneViewGridStep.x -= finalZoomSpeed;
				FG_sceneViewGridStep.y -= finalZoomSpeed;
			}
		}

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		// Cursor mode select
		ImGui::SetCursorScreenPos(canvas_p0);
		RenderCursorModeButtons();

		ImGui::End();
	}

	// Show cursor position in scene view when pressing Alt
	void RenderSceneViewTooltip()
	{
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 positionInGrid = Vector2((inputOutput.MousePos.x - FG_sceneViewCenter.x) / FG_sceneViewGridStep.x, -(inputOutput.MousePos.y - FG_sceneViewCenter.y) / FG_sceneViewGridStep.y);
		std::string cursorXPos = "x: " + std::to_string(positionInGrid.x);
		std::string cursorYPos = "y: " + std::to_string(positionInGrid.y);
		// Mouse Hover Tooltip - Scene View Tooltip			
		ImGui::BeginTooltip();
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text("Scene View Data ");
		ImGui::Separator();
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		// Cursor Position
		ImGui::Text("Cursor Position: ");
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
		ImGui::Text(cursorXPos.c_str());
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
		ImGui::Text(cursorYPos.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::EndTooltip();
	}

	void RenderCursorModeButtons()
	{
		Vector2 startPos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(startPos, Vector2(startPos.x + 360, startPos.y + 46), FL::GetColor32("cursorModeSelectBg"), 0);
		ImGui::GetWindowDrawList()->AddRect(Vector2(startPos.x - 1, startPos.y - 1), Vector2(startPos.x + 360, startPos.y + 46), FL::GetColor32("cursorModeSelectBorder"), 0);
		ImGui::SetCursorScreenPos(Vector2(startPos.x + 10, startPos.y + 5));


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TranslateModeIcon", FL::GetTexture("translate"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
		}
		else
		{
			if (FL::RenderImageButton("#TranslateModeIcon", FL::GetTexture("translate"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Translate Mode (t)");
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#ScaleModeIcon", FL::GetTexture("scale"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::SCALE;
		}
		else
		{
			if (FL::RenderImageButton("#ScaleModeIcon", FL::GetTexture("scale"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::SCALE;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Scale Mode (s)");
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#RotateModeIcon", FL::GetTexture("rotate"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::ROTATE;
		}
		else
		{
			if (FL::RenderImageButton("#RotateModeIcon", FL::GetTexture("rotate"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::ROTATE;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Rotate Mode (r)");
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TileBrushModeIcon", FL::GetTexture("tileBrush"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
		}
		else
		{
			if (FL::RenderImageButton("#TileBrushModeIcon", FL::GetTexture("tileBrush"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Tile Brush Mode (b)");
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TileEraseModeIcon", FL::GetTexture("tileErase"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_ERASE;
		}
		else
		{
			if (FL::RenderImageButton("#TileEraseModeIcon", FL::GetTexture("tileErase"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_ERASE;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Tile Erase Mode (e)");
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#BoxColliderTileDrawModeIcon", FL::GetTexture("tileColliderDraw"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		}
		else
		{
			if (FL::RenderImageButton("#BoxColliderTileDrawModeIcon", FL::GetTexture("tileColliderDraw"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("BoxCollider Draw Mode (c)");
		ImGui::SameLine();

		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#SelectTilesModeIcon", FL::GetTexture("selectTiles"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MULTISELECT;
		}
		else
		{
			if (FL::RenderImageButton("#SelectTilesModeIcon", FL::GetTexture("selectTiles"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MULTISELECT;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Select Tiles Mode (s)");
		ImGui::SameLine();

		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + 35, currentPos.y + 33), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#MoveTilesModeIcon", FL::GetTexture("moveTiles"), Vector2(26, 26), 0, FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MOVE;
		}
		else
		{
			if (FL::RenderImageButton("#MoveTilesModeIcon", FL::GetTexture("moveTiles"), Vector2(26, 26), 0, FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MOVE;
		}
		if (ImGui::IsItemHovered())
			FL::RenderTextToolTip("Move Tiles Mode (m)");
	}
}