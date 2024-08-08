#include "FlatEngine.h"
#include "FlatGui.h"
#include "Component.h"
#include "imgui.h"
#include "Scene.h"
#include "Sprite.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "MappingContext.h"
#include "SceneManager.h"
#include "Project.h"
#include "Vector2.h"

namespace FlatEngine {
	class BoxCollider;
	class Sprite;
	class Transform;
	class MappingContext;
}

using ComponentTypes = FlatEngine::Component::ComponentTypes;
using BoxCollider = FlatEngine::BoxCollider;
using Sprite = FlatEngine::Sprite;
using Transform = FlatEngine::Transform;
using MappingContext = FlatEngine::MappingContext;

namespace FlatGui {

	void MainMenuBar()
	{
		FlatEngine::PushMenuStyles();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project..."))
				{
					// First, save current project
					SaveProject(FlatEngine::F_LoadedProject, FlatEngine::F_LoadedProject.GetPath());
					std::string projectPath = FlatEngine::OpenSaveFileExplorer();
					if (projectPath != "")
					{
						// Then Create New Project and open it
						Project newProject = Project();
						SaveProject(newProject, projectPath);
						OpenProject(projectPath);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Open Project..."))
				{
					// Open Project
					std::string projectPath = FlatEngine::OpenLoadFileExplorer();
					if (projectPath != "")
						OpenProject(projectPath);
				}
				if (ImGui::MenuItem("Save Project"))
				{
					// Save Project
					std::string projectPath = FlatEngine::F_LoadedProject.GetPath();
					SaveProject(FlatEngine::F_LoadedProject, projectPath);
				}
				if (ImGui::MenuItem("Save Project As..."))
				{
					// Save Project As....
					std::string projectPath = FlatEngine::OpenSaveFileExplorer();
					if (projectPath != "")
					{
						SaveProject(FlatEngine::F_LoadedProject, projectPath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene..."))
				{
					// First Save Loaded scene
					FlatEngine::F_SceneManager.SaveCurrentScene();

					std::string scenePath = FlatEngine::OpenSaveFileExplorer();
					if (scenePath != "")
					{
						FlatEngine::F_SceneManager.SaveScene(FlatEngine::F_SceneManager.CreateNewScene(), scenePath);
						ResetHierarchyExpanderTracker();
						FlatEngine::F_SceneManager.LoadScene(scenePath);
						FlatEngine::F_LoadedProject.SetLoadedScenePath(scenePath);
					}
				}

				if (ImGui::MenuItem("Load Scene...", "Ctrl+L"))
				{
					// Load the scene
					std::string scenePath = FlatEngine::OpenLoadFileExplorer();
					if (scenePath != "")
					{
						FlatEngine::F_SceneManager.LoadScene(scenePath);
						FlatEngine::F_LoadedProject.SetLoadedScenePath(scenePath);
					}
				}

				// Save the scene
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					FlatEngine::F_SceneManager.SaveCurrentScene();

				if (ImGui::MenuItem("Save Scene As..."))
				{
					// Save the scene
					std::string scenePath = FlatEngine::OpenSaveFileExplorer();
					if (scenePath != "")
					{
						Scene* currentScene = FlatEngine::GetLoadedScene();
						FlatEngine::F_SceneManager.SaveScene(currentScene, scenePath);
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4"))
					FlatEngine::CloseProgram();

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
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Ball"))
					{
						GameObject ball = FlatEngine::Instantiate("P_Ball", Vector2(0, 0), -1);
						SetFocusedGameObjectID(ball.GetID());
					}
					if (ImGui::MenuItem("Block"))
					{
						GameObject block = FlatEngine::Instantiate("P_Block", Vector2(0, 0), -1);
						SetFocusedGameObjectID(block.GetID());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Components"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						newObject->AddSpriteComponent();
						newObject->SetName("Sprite(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Button"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						newObject->AddButtonComponent();
						newObject->SetName("Button(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Camera"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						newObject->AddCameraComponent();
						newObject->SetName("Camera(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Canvas"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);					
						newObject->AddCanvasComponent();
						newObject->SetName("Canvas(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Animation"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);						
						newObject->AddAnimationComponent();
						Sprite* sprite = newObject->AddSpriteComponent();
						sprite->SetTexture("assets/images/resources/block.png");
						newObject->SetName("Animation(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Audio"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						newObject->AddAudioComponent();
						newObject->SetName("Audio(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Text"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);						
						newObject->AddTextComponent();
						newObject->SetName("Text(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Script"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						newObject->AddScriptComponent();
						newObject->SetName("Script(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CharacterController"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);
						
						newObject->AddCharacterControllerComponent();
						newObject->SetName("CharacterController(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("RigidBody"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);						
						newObject->AddRigidBodyComponent();
						newObject->SetName("RigidBody(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("BoxCollider"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);						
						newObject->AddBoxColliderComponent();
						newObject->SetName("BoxCollider(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CircleCollider"))
					{
						GameObject *newObject = FlatEngine::CreateGameObject(-1);						
						newObject->AddCircleColliderComponent();
						newObject->SetName("CircleCollider(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Prefabs"))
				{
					std::map<std::string, std::vector<GameObject>> prefabs = FlatEngine::GetPrefabs();
					if (prefabs.size() > 0)
					{
						for (std::pair<std::string, std::vector<GameObject>> pair : prefabs)
						{
							if (ImGui::MenuItem(pair.first.c_str()))
							{
								GameObject instantiatedObject = FlatEngine::Instantiate(pair.first, Vector2(0, 0), -1);
								SetFocusedGameObjectID(instantiatedObject.GetID());
							}
						}
					}
					else
						ImGui::Text("Right click GameObject in hierarchy to create a Prefab");

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Asset files"))
				{
					if (ImGui::MenuItem("Mapping Context"))
					{
						std::string path = FlatEngine::OpenSaveFileExplorer();
						if (path != "")
						{
							std::string name = path.substr(path.find_last_of("/\\") + 1);
							MappingContext newContext = MappingContext();
							newContext.SetPath(path);
							newContext.SetName(name);
							FlatEngine::SaveMappingContext(path, newContext);
							FlatEngine::InitializeMappingContexts();
							_showMappingContextEditor = true;
						}
					}
					if (ImGui::MenuItem("Animation"))
					{
						std::string path = FlatEngine::OpenSaveFileExplorer();
						if (path != "")
						{
							std::string name = path.substr(path.find_last_of("/\\") + 1);
							std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
							animationProperties->animationName = name;
							FlatEngine::CreateNewAnimationFile(path);
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
				if (ImGui::MenuItem("Project Settings", NULL, _showSettings))
					_showSettings = !_showSettings;
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		FlatEngine::PopMenuStyles();
	}
}