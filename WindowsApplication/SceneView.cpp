#include "FlatEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"

namespace FlatEngine { namespace FlatGui {

	// Scene view
	// The multiplier for sceneViewGridStep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	ImVec2 sceneViewGridStep = ImVec2(50,50);
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;
	ImVec2 sceneViewScrolling = ImVec2(0,0);
	ImVec2 sceneViewCenter = ImVec2(0, 0);

	void Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		PushWindowStyles();
		ImGui::Begin("Scene View");
		PopWindowStyles();

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

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

		// This will catch our interactions
		ImGui::InvisibleButton("SceneViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			sceneViewScrolling.x += inputOutput.MouseDelta.x;
			sceneViewScrolling.y += inputOutput.MouseDelta.y;
		}

		bool _weightedScroll = true;
		RenderGridView(sceneViewCenter, sceneViewScrolling, _weightedScroll, canvas_p0, canvas_p1, canvas_sz, sceneViewGridStep, ImVec2(SCENE_VIEWPORT_WIDTH/2, SCENE_VIEWPORT_HEIGHT/2));

		// Get currently loaded scene objects
		std::shared_ptr<Scene> loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;

		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::vector<std::shared_ptr<GameObject>>();

		RenderViewObjects(sceneObjects, sceneViewCenter, canvas_p0, canvas_sz, sceneViewGridStep.x);

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
}