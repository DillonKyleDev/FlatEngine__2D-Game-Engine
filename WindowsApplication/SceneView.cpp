#include "FlatEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"

namespace FlatEngine { namespace FlatGui {

	// Scene view
	// The multiplier for sceneViewGridStep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	Vector2 sceneViewGridStep = Vector2(50,50);
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;
	Vector2 sceneViewScrolling = Vector2(0,0);
	Vector2 sceneViewCenter = Vector2(0, 0);

	void Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));

		PushWindowStyles();
		ImGui::Begin("Scene View", 0, 16 | 8);
		PopWindowStyles();

		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set initial viewport dimensions for rendering scene view grid and objects
		if (!_firstSceneRenderPass)
		{
			if (!_sceneHasBeenSet)
			{
				SCENE_VIEWPORT_WIDTH = canvas_sz.x;
				SCENE_VIEWPORT_HEIGHT = canvas_sz.y;
				_sceneHasBeenSet = true;
			}
		}
		_firstSceneRenderPass = false;

		// For calculating scrolling mouse position and what vector to zoom to
		DYNAMIC_VIEWPORT_WIDTH = trunc(canvas_p1.x - canvas_p0.x);
		DYNAMIC_VIEWPORT_HEIGHT = trunc(canvas_p1.y - canvas_p0.y);

		// Get Input and Output
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();
		Vector2 centerOffset = Vector2(SCENE_VIEWPORT_WIDTH / 2, SCENE_VIEWPORT_HEIGHT / 2);
		bool _weightedScroll = true;

		RenderGridView(sceneViewCenter, sceneViewScrolling, _weightedScroll, canvas_p0, canvas_p1, canvas_sz, sceneViewGridStep, centerOffset);

		// Get currently loaded scene objects
		std::shared_ptr<Scene> loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;

		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::vector<std::shared_ptr<GameObject>>();

		RenderViewObjects(sceneObjects, sceneViewCenter, canvas_p0, canvas_sz, sceneViewGridStep.x);

		// This will catch our interactions
		ImGui::SetCursorScreenPos(currentPos);
		ImGui::InvisibleButton("SceneViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			sceneViewScrolling.x += inputOutput.MouseDelta.x;
			sceneViewScrolling.y += inputOutput.MouseDelta.y;
		}

		Vector2 adjustedScrolling = Vector2(sceneViewScrolling.x + centerOffset.x, sceneViewScrolling.y + centerOffset.y);

		// Get scroll amount for changing zoom level of scene view
		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.08f;
		float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
		float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);

		// Change scrolling offset based on mouse position and weight
		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				if (_weightedScroll)
				{
					sceneViewScrolling.x -= trunc(signedMousePosX * weight);
					sceneViewScrolling.y -= trunc(signedMousePosY * weight);
				}
				sceneViewGridStep.x += 1;
				sceneViewGridStep.y += 1;
			}
			else if (scrollInput < 0 && sceneViewGridStep.x > 2 && sceneViewGridStep.y > 2)
			{
				if (_weightedScroll)
				{
					sceneViewScrolling.x += trunc(signedMousePosX * weight);
					sceneViewScrolling.y += trunc(signedMousePosY * weight);
				}
				sceneViewGridStep.x -= 1;
				sceneViewGridStep.y -= 1;
			}
		}

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
}