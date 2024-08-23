#include "FlatEngine.h"
#include "FlatGui.h"
#include "AssetManager.h"
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

namespace FL = FlatEngine;

namespace FlatEngine {
	class BoxCollider;
	class Sprite;
	class Transform;
	class MappingContext;
}

using ComponentTypes = FL::Component::ComponentTypes;
using BoxCollider = FL::BoxCollider;
using Sprite = FL::Sprite;
using Transform = FL::Transform;
using MappingContext = FL::MappingContext;

namespace FlatGui 
{
	void MainMenuBar()
	{
		std::string newScriptModalLabel = "Create Lua Script";
		static bool b_openNewScriptModal = false;
		static bool b_openAnimationModal = false;
		static bool b_openMappingContextModal = false;

		FL::PushMenuStyles();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project..."))
				{
					// First, save current project
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
					std::string projectPath = FL::OpenSaveFileExplorer();
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
					std::string projectPath = FL::OpenLoadFileExplorer();
					if (projectPath != "")
						OpenProject(projectPath);
				}
				if (ImGui::MenuItem("Save Project"))
				{
					// Save Project
					std::string projectPath = FL::F_LoadedProject.GetPath();
					SaveProject(FL::F_LoadedProject, projectPath);
				}
				if (ImGui::MenuItem("Save Project As..."))
				{
					// Save Project As....
					std::string projectPath = FL::OpenSaveFileExplorer();
					if (projectPath != "")
					{
						SaveProject(FL::F_LoadedProject, projectPath);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene..."))
				{
					// First Save Loaded scene
					FL::F_SceneManager.SaveCurrentScene();

					std::string scenePath = FL::OpenSaveFileExplorer();
					if (scenePath != "")
					{
						FL::F_SceneManager.SaveScene(FL::F_SceneManager.CreateNewScene(), scenePath);
						FL::F_SceneManager.LoadScene(scenePath);
						FL::F_LoadedProject.SetLoadedScenePath(scenePath);
					}
				}

				if (ImGui::MenuItem("Load Scene...", "Ctrl+L"))
				{
					// Load the scene
					std::string scenePath = FL::OpenLoadFileExplorer();
					if (scenePath != "")
					{
						FL::F_SceneManager.LoadScene(scenePath);
						FL::F_LoadedProject.SetLoadedScenePath(scenePath);
					}
				}

				// Save the scene
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					FL::F_SceneManager.SaveCurrentScene();

				if (ImGui::MenuItem("Save Scene As..."))
				{
					// Save the scene
					std::string scenePath = FL::OpenSaveFileExplorer();
					if (scenePath != "")
					{
						Scene* currentScene = FL::GetLoadedScene();
						FL::F_SceneManager.SaveScene(currentScene, scenePath);
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4"))
					FL::CloseProgram();

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
			if (ImGui::BeginMenu("Viewports"))
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
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::MenuItem("New GameObject"))
				{
					GameObject* newObject = FL::CreateGameObject(-1);
					SetFocusedGameObjectID(newObject->GetID());
				}
				if (ImGui::BeginMenu("Components"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddSpriteComponent();
						newObject->SetName("Sprite(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Button"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddButtonComponent();
						newObject->SetName("Button(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Camera"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddCameraComponent();
						newObject->SetName("Camera(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Canvas"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);					
						newObject->AddCanvasComponent();
						newObject->SetName("Canvas(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Animation"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddAnimationComponent();
						Sprite* sprite = newObject->AddSpriteComponent();
						sprite->SetTexture("assets/images/resources/block.png");
						newObject->SetName("Animation(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Audio"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddAudioComponent();
						newObject->SetName("Audio(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Text"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddTextComponent();
						newObject->SetName("Text(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Script"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddScriptComponent();
						newObject->SetName("Script(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CharacterController"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						
						newObject->AddCharacterControllerComponent();
						newObject->SetName("CharacterController(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("RigidBody"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddRigidBodyComponent();
						newObject->SetName("RigidBody(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("BoxCollider"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddBoxColliderComponent();
						newObject->SetName("BoxCollider(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					//if (ImGui::MenuItem("CircleCollider"))
					//{
					//	GameObject *newObject = FL::CreateGameObject(-1);						
					//	newObject->AddCircleColliderComponent();
					//	newObject->SetName("CircleCollider(" + std::to_string(newObject->GetID()) + ")");
					//	SetFocusedGameObjectID(newObject->GetID());
					//}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Prefabs"))
				{
					std::map<std::string, std::vector<GameObject>> prefabs = FL::GetPrefabs();
					if (prefabs.size() > 0)
					{
						for (std::pair<std::string, std::vector<GameObject>> pair : prefabs)
						{
							if (ImGui::MenuItem(pair.first.c_str()))
							{
								GameObject instantiatedObject = FL::Instantiate(pair.first, Vector2(0, 0), -1);
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
						b_openMappingContextModal = true;
					}
					if (ImGui::MenuItem("Animation"))
					{
						b_openAnimationModal = true;
					}
					if (ImGui::MenuItem("Lua Script"))
					{						
						b_openNewScriptModal = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::MenuItem("Reload Lua Colors"))
				{
					FL::F_AssetManager.CollectColors();					
				}
				if (ImGui::MenuItem("Reload Lua Textures"))
				{
					FL::F_AssetManager.CollectTextures();					
				}
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
				if (ImGui::MenuItem("Build Project", NULL, _showSettings))
					FL::BuildProject();
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// New Lua Script Modal
		std::string newScriptName = "";
		if (FL::RenderInputModal(newScriptModalLabel.c_str(), "Enter a name for the Lua script:", newScriptName, b_openNewScriptModal))
		{
			FL::CreateNewLuaScript(newScriptName);
		}
		// New Animation Modal
		std::string animationName = "";
		if (FL::RenderInputModal("Create New Animation", "Enter a name for the new Animation file", animationName, b_openAnimationModal))
		{
			std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
			animationProperties->animationName = animationName;
			FL::CreateNewAnimationFile(animationName);
			SaveAnimationData(animationProperties, FL::GetDir("animations") + "/" + animationName + ".anm");
			SetFocusedAnimation(animationProperties);
			_showAnimator = true;
			_showAnimationPreview = true;
		}
		// New Mapping Context Modal
		std::string mappingContextName = "";
		if (FL::RenderInputModal("Create New Mapping Context", "Enter a name for the new Mapping Context", mappingContextName, b_openMappingContextModal))
		{
			FL::CreateNewMappingContextFile(mappingContextName);
			FL::F_selectedMappingContextName = mappingContextName;
			_showMappingContextEditor = true;
		}

		FL::PopMenuStyles();
	}
}