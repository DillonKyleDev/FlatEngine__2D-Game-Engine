#include "FlatEngine.h"
#include "imgui.h"
#include "Scene.h"

namespace FlatEngine { namespace FlatGui {

	void MainMenuBar()
	{
		PushMenuStyles();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project..."))
				{
					// First, save current project
					SaveProject(loadedProject, loadedProject->GetPath());
					// Then Create New Project and open it
					std::shared_ptr<Project> newProject = std::make_shared<Project>();
					std::string projectPath = OpenSaveFileExplorer();
					SaveProject(newProject, projectPath);
					OpenProject(projectPath);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Open Project"))
				{
					// Open Project
					std::string projectPath = OpenLoadFileExplorer();
					OpenProject(projectPath);
				}
				if (ImGui::MenuItem("Save Project"))
				{
					// Save Project
					std::string projectPath = loadedProject->GetPath();
					SaveProject(loadedProject, projectPath);
				}
				if (ImGui::MenuItem("Save Project As..."))
				{
					// Save Project As....
					std::string projectPath = OpenSaveFileExplorer();
					SaveProject(loadedProject, projectPath);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene"))
				{
					// First Save Loaded scene
					sceneManager->SaveCurrentScene();
					std::shared_ptr<Scene> newScene = sceneManager->CreateNewScene();
					sceneManager->LoadScene(newScene->GetPath());
					loadedProject->SetLoadedScenePath(newScene->GetPath());
				}

				if (ImGui::MenuItem("Load Scene", "Ctrl+L"))
				{
					// Load the scene
					std::string scenePath = OpenLoadFileExplorer();
					sceneManager->LoadScene(scenePath);
					loadedProject->SetLoadedScenePath(scenePath);
				}

				// Save the scene
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					sceneManager->SaveCurrentScene();

				if (ImGui::MenuItem("Save Scene As..."))
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
					if (ImGui::MenuItem("ImGui Demo Window", NULL, _showDemoWindow))
						_showDemoWindow = !_showDemoWindow;
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
					if (ImGui::MenuItem("Key Frame Editor", NULL, _showKeyFrameEditor))
						_showKeyFrameEditor = !_showKeyFrameEditor;
					if (ImGui::MenuItem("Animation Preview", NULL, _showAnimationPreview))
						_showAnimationPreview = !_showAnimationPreview;
					if (ImGui::MenuItem("Logger", NULL, _showLogger))
						_showLogger = !_showLogger;
					if (ImGui::MenuItem("Profiler", NULL, _showProfiler))
						_showProfiler = !_showProfiler;
					if (ImGui::MenuItem("Mapping Context Editor", NULL, _showMappingContextEditor))
						_showMappingContextEditor = !_showMappingContextEditor;

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::BeginMenu("Create New..."))
				{
					if (ImGui::MenuItem("GameObject"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Mapping Context"))
					{
						std::string path = OpenSaveFileExplorer();
						std::string name = path.substr(path.find_last_of("/\\") + 1);
						std::shared_ptr<MappingContext> newContext = std::make_shared<MappingContext>();
						newContext->SetPath(path);
						newContext->SetName(name);
						SaveMappingContext(path, newContext);
						InitializeMappingContexts();
						_showMappingContextEditor = true;
					}

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

		PopMenuStyles();
	}
}
}