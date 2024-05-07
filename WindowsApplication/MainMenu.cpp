#include "FlatEngine.h"
#include "imgui.h"
#include "Scene.h"
#include "Sprite.h"
#include "Transform.h"
#include "Vector2.h"

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
					std::string projectPath = OpenSaveFileExplorer();
					if (projectPath != "")
					{
						// Then Create New Project and open it
						std::shared_ptr<Project> newProject = std::make_shared<Project>();
						SaveProject(newProject, projectPath);
						OpenProject(projectPath);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Open Project..."))
				{
					// Open Project
					std::string projectPath = OpenLoadFileExplorer();
					if (projectPath != "")
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
					if (projectPath != "")
					{
						SaveProject(loadedProject, projectPath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene..."))
				{
					// First Save Loaded scene
					sceneManager->SaveCurrentScene();

					std::string scenePath = OpenSaveFileExplorer();
					if (scenePath != "")
					{
						sceneManager->SaveScene(sceneManager->CreateNewScene(), scenePath);
						loadedProject->SetLoadedScenePath(scenePath);
					}
				}

				if (ImGui::MenuItem("Load Scene...", "Ctrl+L"))
				{
					// Load the scene
					std::string scenePath = OpenLoadFileExplorer();
					if (scenePath != "")
					{
						sceneManager->LoadScene(scenePath);
						loadedProject->SetLoadedScenePath(scenePath);
					}
				}

				// Save the scene
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					sceneManager->SaveCurrentScene();

				if (ImGui::MenuItem("Save Scene As..."))
				{
					// Save the scene
					std::string scenePath = OpenSaveFileExplorer();
					if (scenePath != "")
					{
						std::shared_ptr<Scene> currentScene = sceneManager->GetLoadedScene();
						sceneManager->SaveScene(currentScene, scenePath);
					}
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
				if (ImGui::BeginMenu("GameObjects"))
				{
					if (ImGui::MenuItem("New GameObject"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Ball"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						std::shared_ptr<Transform> transform = std::static_pointer_cast<Transform>(newObject->AddComponent(ComponentTypes::Transform));
						newObject->AddComponent(ComponentTypes::RigidBody);
						newObject->AddComponent(ComponentTypes::CircleCollider);
						std::shared_ptr<Sprite> sprite = std::static_pointer_cast<Sprite>(newObject->AddComponent(ComponentTypes::Sprite));
						sprite->SetTexture("assets/images/resources/ball.png");
						transform->SetScale(Vector2(0.02f, 0.02f));
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Block"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						std::shared_ptr<Transform> transform = std::static_pointer_cast<Transform>(newObject->AddComponent(ComponentTypes::Transform));
						newObject->AddComponent(ComponentTypes::RigidBody);
						newObject->AddComponent(ComponentTypes::CircleCollider);
						std::shared_ptr<Sprite> sprite = std::static_pointer_cast<Sprite>(newObject->AddComponent(ComponentTypes::Sprite));
						sprite->SetTexture("assets/images/resources/block.png");
						
						SetFocusedGameObjectID(newObject->GetID());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Components"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Sprite);
						newObject->SetName("Sprite(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Button"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Button);
						newObject->SetName("Button(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Camera"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Camera);
						newObject->SetName("Camera(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Canvas"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Canvas);
						newObject->SetName("Canvas(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Animation"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Animation);											
						std::shared_ptr<Sprite> sprite = std::static_pointer_cast<Sprite>(newObject->AddComponent(ComponentTypes::Sprite));
						sprite->SetTexture("assets/images/resources/block.png");
						newObject->SetName("Animation(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Audio"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Audio);
						newObject->SetName("Audio(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Text"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::Text);
						newObject->SetName("Text(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Script"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Script);
						newObject->SetName("Script(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CharacterController"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::CharacterController);
						newObject->SetName("CharacterController(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("RigidBody"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::RigidBody);
						newObject->SetName("RigidBody(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("BoxCollider"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::BoxCollider);
						newObject->SetName("BoxCollider(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CircleCollider"))
					{
						std::shared_ptr<GameObject> newObject = CreateGameObject(-1);
						newObject->AddComponent(ComponentTypes::Transform);
						newObject->AddComponent(ComponentTypes::CircleCollider);
						newObject->SetName("CircleCollider(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Asset files"))
				{
					if (ImGui::MenuItem("Mapping Context"))
					{
						std::string path = OpenSaveFileExplorer();
						if (path != "")
						{
							std::string name = path.substr(path.find_last_of("/\\") + 1);
							std::shared_ptr<MappingContext> newContext = std::make_shared<MappingContext>();
							newContext->SetPath(path);
							newContext->SetName(name);
							SaveMappingContext(path, newContext);
							InitializeMappingContexts();
							_showMappingContextEditor = true;
						}
					}
					if (ImGui::MenuItem("Animation"))
					{
						std::string path = OpenSaveFileExplorer();
						if (path != "")
						{
							std::string name = path.substr(path.find_last_of("/\\") + 1);
							std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
							animationProperties->animationName = name;
							CreateNewAnimationFile(path);
							SaveAnimationFile(animationProperties, path);
							//LoadAnimationFile(path);
							SetFocusedAnimation(animationProperties);
							
							_showAnimator = true;
							_showAnimationPreview = true;
						}
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