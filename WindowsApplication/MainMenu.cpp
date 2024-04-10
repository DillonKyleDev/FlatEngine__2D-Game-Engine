#include "FlatEngine.h"
#include "imgui.h"

namespace FlatEngine { namespace FlatGui {

	void MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
					sceneManager->CreateNewScene();

				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
				{
					// Load the scene
					std::string scenePath = OpenLoadFileExplorer();
					sceneManager->LoadScene(scenePath);
				}

				// Save the scene
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					sceneManager->SaveCurrentScene();

				if (ImGui::MenuItem("Save Scene As.."))
				{
					// Save the scene
					std::string scenePath = OpenSaveFileExplorer();
					std::shared_ptr<Scene> currentScene = sceneManager->GetLoadedScene();
					sceneManager->SaveScene(currentScene, scenePath);
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4"))
					CloseProgram();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					if (ImGui::MenuItem("Scene View", NULL, _showSceneView))
						_showSceneView = !_showSceneView;
					if (ImGui::MenuItem("Game View", NULL, _showGameView))
						_showGameView = !_showGameView;
					if (ImGui::MenuItem("Hierarchy", NULL, _showHierarchy))
						_showHierarchy = !_showHierarchy;
					if (ImGui::MenuItem("Inspector", NULL, _showInspector))
						_showInspector = !_showInspector;
					if (ImGui::MenuItem("Animator", NULL, _showAnimator))
						_showAnimator = !_showAnimator;
					if (ImGui::MenuItem("Animation Preview", NULL, _showAnimationPreview))
						_showAnimationPreview = !_showAnimationPreview;
					if (ImGui::MenuItem("Logger", NULL, _showLogger))
						_showLogger = !_showLogger;
					if (ImGui::MenuItem("Profiler", NULL, _showProfiler))
						_showProfiler = !_showProfiler;

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::BeginMenu("Widgets"))
				{
					if (ImGui::BeginMenu("Scene View Icon Transparency"))
					{
						// Icon transparency slider
						ImGui::SliderInt("Scene View Icon Transparency", &iconTransparency, 0, 255, "%d");

						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		bool show_demo_window = true;
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}
}
}