#include "FlatGui.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "UIManager.h"
#include "WidgetsManager.h"
#include "SceneManager.h"
#include "PrefabManager.h"
#include "Texture.h"
#include "Project.h"
#include "Logger.h"
#include "Component.h"
#include "Script.h"
#include "Transform.h"
#include "Sprite.h"
#include "Text.h"
#include "Scene.h"
#include "Camera.h"
#include "Sound.h"
#include "Button.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "MappingContext.h"
#include "Process.h"
#include <Windows.h>
#include "ECSManager.h"

#include "Texture.h"

#include "imgui.h"
#include <math.h>
#include <cmath>
#include <string>
#include <shobjidl.h> 
#include <Commdlg.h>
#include "implot.h"
#include "implot_internal.h"
#include "imgui_internal.h"
#include <fstream>
#include <vector>
#include "ImSequencer.h"
#include <filesystem>
#include <windows.h> // For getting directory name


/*
######################################
######							######
######    Gui Implimentation    ######
######                          ######
######################################
*/

using GameObject = FlatEngine::GameObject;
using Button = FlatEngine::Button;
using Component = FlatEngine::Component;
using MappingContext = FlatEngine::MappingContext;
using Transform = FlatEngine::Transform;
using Sprite = FlatEngine::Sprite;
using Camera = FlatEngine::Camera;
using Canvas = FlatEngine::Canvas;
using Text = FlatEngine::Text;
using BoxCollider = FlatEngine::BoxCollider;
using CircleCollider = FlatEngine::CircleCollider;
using Sound = FlatEngine::Sound;
using ComponentTypes = Component::ComponentTypes;

namespace FlatGui 
{
	// For window styles
	ImDrawList* drawList = nullptr;

	// Managers
	WidgetsManager widgetsManager = WidgetsManager();

	// FlatGui Variables
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = nullptr;
	GameObject objectForFocusedAnimation = GameObject(nullptr);
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	long previewAnimationStartTime = 0;
	long previewAnimationTime = 0;
	bool _playPreviewAnimation = false;
	long FocusedGameObjectID = -1;
	std::shared_ptr<GameObject> playerObject = nullptr;


	// Frame Counter
	int framesDrawn = 0;

	// Settings
	int iconTransparency = 50;

	// Hierarchy	
	std::map<long, bool> leafExpandedTracker = std::map<long, bool>();

	// Window Visibility
	bool _showDemoWindow = false;
	bool _showSceneView = true;
	bool _showGameView = true;
	bool _showHierarchy = true;
	bool _showInspector = true;
	bool _showAnimator = false;
	bool _showAnimationPreview = false;
	bool _showKeyFrameEditor = false;
	bool _showLogger = true;
	bool _showProfiler = true;
	bool _showMappingContextEditor = false;
	bool _showSettings = false;

	Vector2 worldCenterPoint = Vector2(0, 0);

	void Init()
	{
		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Remove the reference to the imgui.ini file for layout since we only need that in Engine mode and
			// we don't want to have to include it in the final release build anyway.
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.IniFilename = NULL;

			// Set fullscreen here for now
			Window::SetFullscreen(FlatEngine::F_LoadedProject.IsFullscreen());
		}
		else
			FlatEngine::CreateNewScene();
	}

	void SetupProfilerProcesses()
	{
		if (_showProfiler && FlatEngine::_isDebugMode)
		{
			// Add Profiler Processes
			// 						
			FlatEngine::AddProfilerProcess("Render");
			FlatEngine::AddProfilerProcess("Render Present");
			// RenderMainMenuBar()						
			//FlatEngine::AddProfilerProcess("Render Main Menu Bar");
			// RenderToolbar()							
			//FlatEngine::AddProfilerProcess("Render Toolbar");

			// RenderHierarchy()
			if (_showHierarchy)
				FlatEngine::AddProfilerProcess("Render Hierarchy");
			else
				FlatEngine::RemoveProfilerProcess("Render Hierarchy");

			// RenderInspector()
			if (_showInspector)
				FlatEngine::AddProfilerProcess("Render Inspector");
			else
				FlatEngine::RemoveProfilerProcess("Render Inspector");

			// Game_RenderView
			if (_showGameView)
				FlatEngine::AddProfilerProcess("Render Game View");
			else
				FlatEngine::RemoveProfilerProcess("Render Game View");

			// Scene_RenderView
			if (_showSceneView)
				FlatEngine::AddProfilerProcess("Render Scene View");
			else
				FlatEngine::RemoveProfilerProcess("Render Scene View");

			// RenderAnimator
			if (_showAnimator)
				FlatEngine::AddProfilerProcess("Render Animator");
			else
				FlatEngine::RemoveProfilerProcess("Render Animator");

			// RenderAnimationPreview
			if (_showAnimationPreview)
				FlatEngine::AddProfilerProcess("Render Animation Preview");
			else
				FlatEngine::RemoveProfilerProcess("Render Animation Preview");

			// RenderKeyFrameEditor
			if (_showKeyFrameEditor)
				FlatEngine::AddProfilerProcess("Render Key Frame Editor");
			else
				FlatEngine::RemoveProfilerProcess("Render Key Frame Editor");

			// RenderLog
			if (_showLogger)
				FlatEngine::AddProfilerProcess("Render Log");
			else
				FlatEngine::RemoveProfilerProcess("Render Log");

			// RenderProfiler
			if (_showProfiler)
				FlatEngine::AddProfilerProcess("Render Profiler");
			else
				FlatEngine::RemoveProfilerProcess("Render Profiler");

			// RenderMappingContextEditor
			if (_showMappingContextEditor)
				FlatEngine::AddProfilerProcess("Render Mapping Context Editor");
			else
				FlatEngine::RemoveProfilerProcess("Render Mapping Context Editor");

			// RenderSettings
			if (_showSettings)
				FlatEngine::AddProfilerProcess("Render Settings");
			else
				FlatEngine::RemoveProfilerProcess("Render Settings");

			// Collision Testing
			FlatEngine::AddProfilerProcess("Collision Testing");
		}
	}

	void RunOnceAfterInitialization()
	{
		static bool _initialized = false;
		static bool _hasRunOnce = false;

		if (_initialized && !_hasRunOnce)
		{
			// Initialize Mapping Contexts
			FlatEngine::InitializeMappingContexts();

			// Initialize prefab objects
			FlatEngine::prefabManager->InitializePrefabs();

			// Initialize GameLoop handlers (colliders, rigidbodies, scripts)
			//FlatEngine::F_Application->GetGameLoop()->CollectPhysicsBodies();

			// Hierarchy management
			if (FlatEngine::_isDebugMode)
				ResetHierarchyExpanderTracker();

			SetupProfilerProcesses();

			_hasRunOnce = true;
		}

		_initialized = true;
	}


	void Cleanup()
	{
		// Remove Profiler Processes
		if (FlatEngine::_isDebugMode)
		{
			FlatEngine::RemoveProfilerProcess("Render");
			FlatEngine::RemoveProfilerProcess("Render Present");
		}
	}

	void SetFocusedGameObjectID(long ID)
	{
		FocusedGameObjectID = ID;
		if (ID != -1)
		{
			GameObject focusedObject = FlatEngine::GetObjectById(ID);
			/*FlatEngine::F_Lua["this_object"] = &(*FlatEngine::GetObjectById(ID));*/
			FlatEngine::LuaTesting((*FlatEngine::GetObjectById(ID)));
			Animation* animationComponent = focusedObject.GetAnimation();
			std::string animationPath = "";

			if (animationComponent != nullptr)
				animationPath = animationComponent->GetAnimationPath();

			// If applicable to the current animation, create a copy of the focused GameObject to be used for the animator window.
			if (_showAnimator && FocusedAnimation != nullptr &&
				animationComponent != nullptr && animationPath == FocusedAnimation->animationPath)
			{
				std::vector<GameObject> animatorObjects = std::vector<GameObject>();
				animatorObjects.clear();
				//objectForFocusedAnimation = GameObject(FlatEngine::GetObjectById(ID), animatorObjects, FlatEngine::GetLoadedScene()->GetSceneObjects(), -1);
				FlatEngine::Transform* transform = objectForFocusedAnimation.GetTransform();
				transform->SetPosition(Vector2(0, 0));
				animatorObjects.push_back(&objectForFocusedAnimation);
				//FlatEngine::GetLoadedScene()->SetAnimatorPreviewObjects(animatorObjects); // FIX LATER
			}
		}
	}

	long GetFocusedGameObjectID()
	{
		return FocusedGameObjectID;
	}

	void DestroySelf(std::shared_ptr<FlatEngine::GameObject> thisObject)
	{
		FlatEngine::DeleteGameObject(thisObject->GetID());
	}

	// Project Management
	void OpenProject(std::string path)
	{
		Project newProject = Project();
		newProject.SetPath(path);

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Open file in in mode
		file_obj.open(path, std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj.good())
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		if (file_obj.good())
		{
			// Go from string to json object
			json fileContentJson = json::parse(fileContent);

			if (fileContentJson["Project Properties"][0] != "NULL")
			{
				// Getting data from the json 
				// auto properties = fileContentJson["Animation Properties"];
				// std::string name = properties[0]["name"];

				// Loop through the saved Properties in the JSON file
				for (int i = 0; i < fileContentJson["Project Properties"].size(); i++)
				{
					// Get data from the loaded object
					json currentObjectJson = fileContentJson["Project Properties"][i];

					// Open items
					if (currentObjectJson.contains("path"))
						newProject.SetPath(currentObjectJson["path"]);
					if (currentObjectJson.contains("loadedScenePath"))
						newProject.SetLoadedScenePath(currentObjectJson["loadedScenePath"]);
					if (currentObjectJson.contains("loadedAnimationPath"))
						newProject.SetLoadedPreviewAnimationPath(currentObjectJson["loadedAnimationPath"]);
					if (currentObjectJson.contains("focusedGameObjectID"))
						newProject.SetFocusedGameObjectID(currentObjectJson["focusedGameObjectID"]);

					// Scene Scrolling + Grid Step
					Vector2 sceneViewScroll = Vector2(0, 0);
					if (currentObjectJson.contains("sceneViewScrollingX"))
						sceneViewScroll.x = currentObjectJson["sceneViewScrollingX"];
					if (currentObjectJson.contains("sceneViewScrollingY"))
						sceneViewScroll.y = currentObjectJson["sceneViewScrollingY"];
					newProject.SetSceneViewScrolling(sceneViewScroll);
					Vector2 sceneViewGridStep = Vector2(0, 0);
					if (currentObjectJson.contains("sceneViewGridStepX"))
						sceneViewGridStep.x = currentObjectJson["sceneViewGridStepX"];
					if (currentObjectJson.contains("sceneViewGridStepY"))
						sceneViewGridStep.y = currentObjectJson["sceneViewGridStepY"];
					newProject.SetSceneViewGridStep(sceneViewGridStep);

					// Show/hide windows
					if (currentObjectJson.contains("_showSceneView"))
						_showSceneView = currentObjectJson["_showSceneView"];
					if (currentObjectJson.contains("_showGameView"))
						_showGameView = currentObjectJson["_showGameView"];
					if (currentObjectJson.contains("_showHierarchy"))
						_showHierarchy = currentObjectJson["_showHierarchy"];
					if (currentObjectJson.contains("_showInspector"))
						_showInspector = currentObjectJson["_showInspector"];
					if (currentObjectJson.contains("_showAnimator"))
						_showAnimator = currentObjectJson["_showAnimator"];
					if (currentObjectJson.contains("_showAnimationPreview"))
						_showAnimationPreview = currentObjectJson["_showAnimationPreview"];
					if (currentObjectJson.contains("_showKeyFrameEditor"))
						_showKeyFrameEditor = currentObjectJson["_showKeyFrameEditor"];
					if (currentObjectJson.contains("_showLogger"))
						_showLogger = currentObjectJson["_showLogger"];
					if (currentObjectJson.contains("_showProfiler"))
						_showProfiler = currentObjectJson["_showProfiler"];
					if (currentObjectJson.contains("_showMappingContextEditor"))
						_showMappingContextEditor = currentObjectJson["_showMappingContextEditor"];

					// Settings
					if (currentObjectJson.contains("_clearLogBuffer"))
					{
						_clearBufferEveryFrame = currentObjectJson["_clearLogBuffer"];
						if (_clearBufferEveryFrame)
						{
							FlatEngine::F_Logger.ClearBuffer();
						}
					}
					if (currentObjectJson.contains("_autoSave"))
						newProject.SetAutoSave(currentObjectJson["_autoSave"]);
					if (currentObjectJson.contains("physicsSystem"))
						newProject.SetPhysicsSystem(currentObjectJson["physicsSystem"]);
					if (currentObjectJson.contains("collisionDetection"))
						newProject.SetCollisionDetection(currentObjectJson["collisionDetection"]);
					if (currentObjectJson.contains("resolutionWidth") && currentObjectJson.contains("resolutionHeight"))
						newProject.SetResolution(Vector2(currentObjectJson["resolutionWidth"], currentObjectJson["resolutionHeight"]));
					if (currentObjectJson.contains("_fullscreen"))
						newProject.SetFullscreen(currentObjectJson["_fullscreen"]);
					if (currentObjectJson.contains("_vsyncEnabled"))
						newProject.SetVsyncEnabled(currentObjectJson["_vsyncEnabled"]);
				}
			}
		}


		if (newProject.GetLoadedPreviewAnimationPath() != "")
			SetFocusedAnimation(FlatEngine::LoadAnimationFile(newProject.GetLoadedPreviewAnimationPath()));
		Vector2 scrolling = newProject.GetSceneViewScrolling();
		FG_sceneViewScrolling = scrolling;
		Vector2 gridStep = newProject.GetSceneViewGridStep();
		FG_sceneViewGridStep = gridStep;

		if (newProject.GetFocusedGameObjectID() != -1 && FlatEngine::GetObjectById(newProject.GetFocusedGameObjectID()) != nullptr)
			SetFocusedGameObjectID(newProject.GetFocusedGameObjectID());

		if (newProject.GetLoadedScenePath() != "")
			FlatEngine::LoadScene(newProject.GetLoadedScenePath());
		else
			FlatEngine::CreateNewScene();

		// Set loaded project
		FlatEngine::SetLoadedProject(newProject);
	}

	void SaveProject(Project project, std::string path)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our gameObject json objects
		json projectProperties;

		// Create Animation Property Json data object
		json animationName = json::object({
			{ "path", path },
			{ "loadedScenePath", project.GetLoadedScenePath()},
			{ "loadedAnimationPath", project.GetLoadedPreviewAnimationPath()},
			{ "focusedGameObjectID", GetFocusedGameObjectID() },
			{ "sceneViewScrollingX", FG_sceneViewScrolling.x },
			{ "sceneViewScrollingY", FG_sceneViewScrolling.y },
			{ "sceneViewGridStepX", FG_sceneViewGridStep.x },
			{ "sceneViewGridStepY", FG_sceneViewGridStep.y },
			{ "_showSceneView", _showSceneView },
			{ "_showGameView", _showGameView },
			{ "_showHierarchy", _showHierarchy },
			{ "_showInspector", _showInspector },
			{ "_showAnimator", _showAnimator },
			{ "_showAnimationPreview", _showAnimationPreview },
			{ "_showKeyFrameEditor", _showKeyFrameEditor },
			{ "_showLogger", _showLogger },
			{ "_showProfiler", _showProfiler },
			{ "_showMappingContextEditor", _showMappingContextEditor },
			{ "_clearLogBuffer", _clearBufferEveryFrame },
			{ "_autoSave", FlatEngine::F_LoadedProject.AutoSaveOn() },
			{ "physicsSystem", FlatEngine::F_LoadedProject.GetPhysicsSystem() },
			{ "collisionDetection", FlatEngine::F_LoadedProject.GetCollisionDetection() },
			{ "resolutionWidth", FlatEngine::F_LoadedProject.GetResolution().x },
			{ "resolutionHeight", FlatEngine::F_LoadedProject.GetResolution().y },
			{ "_fullscreen", FlatEngine::F_LoadedProject.IsFullscreen() },
			{ "_vsyncEnabled", FlatEngine::F_LoadedProject.IsVsyncEnabled() },
			});
		projectProperties.push_back(animationName);

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"Project Properties", projectProperties } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}


	void RenderProjectHub(bool& b_projectSelected, std::string &projectPath)
	{
		// Get all project files in the projects folder to present in the project selection screen
		std::vector<Project> projects = std::vector<Project>();

		std::string path = "Source\\projects";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{			
			json contextData = FlatEngine::LoadFileData(entry.path().string());
			if (contextData != NULL)
			{
				auto projectProperties = contextData["Project Properties"][0];
				Project project = Project();
				project.SetPath(FlatEngine::CheckJsonString(contextData["Project Properties"][0], "path", "Project Hub project property check."));
				projects.push_back(project);
			}
		}

		bool b_isOpen = true;
		FlatEngine::SetNextViewportToFillWindow();  // Maximize viewport
		FlatEngine::BeginWindow("Project Hub", b_isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		
		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + (float)FlatEngine::F_selectProjectImage.GetHeight());

		ImGui::GetWindowDrawList()->AddRectFilled(canvas_p0, canvas_p1, ImGui::GetColorU32(FlatEngine::F_selectProjectBgColor), 5);
		ImGui::Image(FlatEngine::F_selectProjectImage.GetTexture(), Vector2((float)FlatEngine::F_selectProjectImage.GetWidth(), (float)FlatEngine::F_selectProjectImage.GetHeight()));

		ImGui::Separator();
		ImGui::Separator();
			
		FlatEngine::BeginWindowChild("Projects");
	
		for (Project project : projects)
		{
			std::string path = project.GetPath();
			if (FlatEngine::RenderButton(FlatEngine::GetFilenameFromPath(path), Vector2(ImGui::GetContentRegionAvail().x, 40)))
			{
				b_projectSelected = true;
				projectPath = path;
			}
		}

		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + ImGui::GetContentRegionAvail().y - 57));

		ImGui::Text("");
		ImGui::Separator();

		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x - 100, ImGui::GetCursorScreenPos().y + 4));

		if (FlatEngine::RenderButton("New Project", Vector2(100, 30)))
		{
			std::string newProjectPath = FlatEngine::OpenSaveFileExplorer();
			if (newProjectPath != "")
			{
				Project newProject = Project();
				SaveProject(newProject, newProjectPath);
				OpenProject(newProjectPath);
			}
		}

		FlatEngine::EndWindowChild();
		FlatEngine::EndWindow();
	}

	void AddViewports()
	{
		// ImGui Demo Window
		if (_showDemoWindow)
			ImGui::ShowDemoWindow(&_showDemoWindow);

		float startTime = (float)FlatEngine::GetEngineTime();
		MainMenuBar();
		FlatEngine::AddProcessData("Render Main Menu Bar", (float)FlatEngine::GetEngineTime() - startTime);

		startTime = (float)FlatEngine::GetEngineTime();
		RenderToolbar();
		FlatEngine::AddProcessData("Render Toolbar", (float)FlatEngine::GetEngineTime() - startTime);
		
		if (_showHierarchy)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderHierarchy();
			FlatEngine::AddProcessData("Render Hierarchy", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showInspector)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderInspector();
			FlatEngine::AddProcessData("Render Inspector", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showGameView)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			FlatEngine::Game_RenderView();
			FlatEngine::AddProcessData("Render Game View", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showSceneView)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			Scene_RenderView();
			FlatEngine::AddProcessData("Render Scene View", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showAnimator)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderAnimator();
			FlatEngine::AddProcessData("Render Animator", (float)FlatEngine::GetEngineTime() - startTime);
		}
		
		if (_showAnimationPreview)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderAnimationPreview();
			FlatEngine::AddProcessData("Render Animation Preview", (float)FlatEngine::GetEngineTime() - startTime);
		}
		
		if (_showKeyFrameEditor)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderKeyFrameEditor();
			FlatEngine::AddProcessData("Render Key Frame Editor", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showLogger)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderLog();
			FlatEngine::AddProcessData("Render Log", (float)FlatEngine::GetEngineTime() - startTime);
		}
	
		if (_showProfiler)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderProfiler();
			FlatEngine::AddProcessData("Render Profiler", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showMappingContextEditor)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderMappingContextEditor();
			FlatEngine::AddProcessData("Render Mapping Context Editor", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showSettings)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderSettings();
			FlatEngine::AddProcessData("Render Settings", (float)FlatEngine::GetEngineTime() - startTime);
		}
	}


	// General in order to be used with multiple views that render objects (animation preview, scene view, etc)
	void RenderGridView(Vector2& centerPoint, Vector2 &scrolling, bool _weightedScroll, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, Vector2 &step, Vector2 centerOffset)
	{
		drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));

		// Our grid step determines the largest gap between each grid point so our centerpoints must fall on
		// one of those step locations. We get the total grid steps that will render given the current viewport
		// size and divide that by two to get the closest spot to the center of the viewport. It's okay that this
		// is not exactly center at all, the viewport width will never be the perfect size, we just need a starting
		// point and for that point. We need to update this value every pass of the scene view because the step
		// value will change over time and we need to keep these in sync.          
		// 
		//                   V
		// |  |  |  |  |  |  |  |  |  |  |  |  |

		// X = 0 starts the drawing at the left most edge of the entire app window.

		// Draw horizontal grid lines
		for (float x = trunc(fmodf(scrolling.x + canvas_p0.x, step.y)); x < canvas_p0.x + canvas_sz.x; x += step.y)
			FlatEngine::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, drawList);
		// Draw vertical grid lines
		for (float y = trunc(fmodf(scrolling.y + canvas_p0.y, step.y)); y < canvas_p0.y + canvas_sz.y; y += step.y)
			FlatEngine::DrawLine(Vector2(canvas_p0.x, y), Vector2(canvas_p1.x, y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, drawList);

		// Draw our x and y axis blue and green lines
		//
		float divX = trunc(scrolling.x / step.x);
		float modX = fmodf(scrolling.x, step.x);
		float offsetX = (step.x * divX) + modX;
		float divY = trunc(scrolling.y / step.y);
		float modY = fmodf(scrolling.y, step.y);
		float offsetY = (step.y * divY) + modY;

		// Blue, green and pink colors for axis and center
		Vector4 xColor = Vector4(0.2f, 0.2f, 0.9f, 1.0f);
		Vector4 yColor = Vector4(0, 0.82f, 0.14f, 1.0f);
		Vector4 centerColor = Vector4(1.0f, 0, 0.96f, 1.0f);

		centerPoint = Vector2(offsetX + canvas_p0.x, offsetY + canvas_p0.y);
		float drawYAxisAt = centerPoint.x;
		float drawXAxisAt = centerPoint.y;

		// x axis bounds check + color change (lighten) if out of bounds
		if (centerPoint.x > canvas_p1.x - 1)
		{
			drawYAxisAt = canvas_p1.x - 1;
			yColor = Vector4(0, 0.82f, 0.14f, 0.4f);
		}
		else if (centerPoint.x < canvas_p0.x)
		{
			drawYAxisAt = canvas_p0.x;
			yColor = Vector4(0, 0.82f, 0.14f, 0.4f);
		}
		// y axis bounds check + color change (lighten) if out of bounds
		if (centerPoint.y > canvas_p1.y - 1)
		{
			drawXAxisAt = canvas_p1.y - 1;
			xColor = Vector4(0.07f, 0.07f, 0.8f, 0.58f);
		}
		else if (centerPoint.y < canvas_p0.y)
		{
			drawXAxisAt = canvas_p0.y;
			xColor = Vector4(0.07f, 0.07f, 0.8f, 0.58f);
		}


		// Draw the axis and center point
		FlatEngine::DrawLine(Vector2(drawYAxisAt, canvas_p0.y), Vector2(drawYAxisAt, canvas_p1.y), yColor, 1.0f, drawList);
		FlatEngine::DrawLine(Vector2(canvas_p0.x, drawXAxisAt), Vector2(canvas_p1.x, drawXAxisAt), xColor, 1.0f, drawList);
		FlatEngine::DrawPoint(Vector2(centerPoint.x, centerPoint.y), centerColor, drawList);
		//DrawLine(sceneViewCenter, Vector2(sceneViewCenter.x + 40, sceneViewCenter.y + 40), whiteColor, 3, drawList);
	}

	void RenderViewObjects(std::vector<GameObject> objects, Vector2 centerPoint, Vector2 canvas_p0, Vector2 canvas_sz, float step)
	{
		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 &2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, FlatEngine::F_maxSpriteLayers + 5);

		// Loop through scene objects
		for (GameObject object : objects)
		{
			if (object.IsActive())
				RenderViewObject(object, centerPoint, canvas_p0, canvas_sz, step, draw_list, drawSplitter);
		}

		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}

	void RenderViewObject(GameObject self, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_sz, float step, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter)
	{
		Transform* transform = self.GetTransform();
		Sprite* sprite = self.GetSprite();
		Camera* camera = self.GetCamera();
		Button* button = self.GetButton();
		Canvas* canvas = self.GetCanvas();
		Text* text = self.GetText();
		std::vector<BoxCollider*> boxColliders = self.GetBoxColliders();
		std::vector<CircleCollider*> circleColliders = self.GetCircleColliders();

		// Check if each object has a Transform component
		if (transform != nullptr)
		{
			long focusedObjectID = GetFocusedGameObjectID();
			Vector2 position = transform->GetTruePosition();
			Vector2 origin = transform->GetOrigin();
			Vector2 transformScale = transform->GetScale();
			float rotation = transform->GetRotation();
			Vector2 scale = transform->GetScale();

			// If it has a sprite component, render that sprite texture at the objects transform position with offsets
			if (sprite != nullptr && sprite->GetTexture() != nullptr)
			{
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float spriteTextureWidth = (float)sprite->GetTextureWidth();
				float spriteTextureHeight = (float)sprite->GetTextureHeight();
				Vector2 spriteScale = sprite->GetScale();
				Vector2 pivotOffset = sprite->GetPivotOffset();
				Vector2 spriteOffset = sprite->GetOffset();
				bool _spriteScalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();
				Vector4 tintColor = sprite->GetTintColor();
				std::string invisibleButtonID = "GameObjectSelectorButton_" + std::to_string(sprite->GetID());

				// Get Input and Output
				ImGuiIO& inputOutput = ImGui::GetIO();

				Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x - canvas_p0.x + (position.x * step) - ((pivotOffset.x * FlatEngine::F_spriteScaleMultiplier * step) * scale.x * spriteScale.x), FG_sceneViewCenter.y - canvas_p0.y - (position.y * step - 20) - ((pivotOffset.y * FlatEngine::F_spriteScaleMultiplier * step) * scale.y * spriteScale.y));
				ImGui::SetCursorPos(positionOnScreen);
				//// This will catch our interactions  - 4096 for overlap or keyword if it works
				ImGui::SetNextItemAllowOverlap();
				ImGui::InvisibleButton(invisibleButtonID.c_str(), Vector2(spriteTextureWidth * FlatEngine::F_spriteScaleMultiplier * step * scale.x * spriteScale.x, spriteTextureHeight * FlatEngine::F_spriteScaleMultiplier * step * scale.y * spriteScale.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
				const bool is_hovered = ImGui::IsItemHovered(); // Hovered
				const bool is_active = ImGui::IsItemActive();   // Held
				const bool is_clicked = ImGui::IsItemClicked();

				if (is_clicked)
					SetFocusedGameObjectID(sprite->GetParentID());

				// Show cursor position in scene view when pressing Alt
				if (is_hovered && inputOutput.KeyAlt)
					RenderSceneViewTooltip();

				// Add the same behavior as the sceneview grid so pan and zoom behaviors are not disabled when view entirely obstructed by sprite
				////////////////////////
				if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
				{
					FG_sceneViewScrolling.x += inputOutput.MouseDelta.x;
					FG_sceneViewScrolling.y += inputOutput.MouseDelta.y;
				}
				// Get scroll amount for changing zoom level of scene view
				Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
				float scrollInput = inputOutput.MouseWheel;
				float weight = 0.01f;
				float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
				float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
				bool _weightedScroll = true;
				float zoomSpeed = 0.1f;
				float zoomMultiplier = 10;
				float finalZoomSpeed = zoomSpeed;

				if (ImGui::GetIO().KeyCtrl)
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
				//////////////////

				// Change the draw channel for the scene object
				if (renderOrder <= FlatEngine::F_maxSpriteLayers && renderOrder >= 0)
					drawSplitter->SetCurrentChannel(draw_list, renderOrder);
				else
					drawSplitter->SetCurrentChannel(draw_list, 0);

				// Draw the texture
				FlatEngine::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, pivotOffset, Vector2(transformScale.x * spriteScale.x, transformScale.y * spriteScale.y), _spriteScalesWithZoom, step, draw_list, rotation, ImGui::GetColorU32(tintColor));
			}

			// If it has a text component, render that text texture at the objects transform position
			if (text != nullptr)
			{
				Texture textTexture = text->GetTexture();
				text->LoadText();
				float textWidth = (float)textTexture.GetWidth();
				float textHeight = (float)textTexture.GetHeight();
				int renderOrder = text->GetRenderOrder();
				Vector2 offset = text->GetOffset();
				bool _spriteScalesWithZoom = true;

				// If there is a valid Texture loaded into the Sprite Component
				if (textTexture.GetTexture() != nullptr)
				{
					// Change the draw channel for the scene object
					if (renderOrder <= FlatEngine::F_maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					FlatEngine::AddImageToDrawList(textTexture.GetTexture(), position, FG_sceneViewCenter, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, FG_sceneViewGridStep.x, draw_list, rotation);
				}
			}

			// Renders the camera
			if (camera != nullptr)
			{
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = scrolling.x + (position.x * FG_sceneViewGridStep.x) - (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
				float cameraTopEdge = scrolling.y + (-position.y * FG_sceneViewGridStep.y) - (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);
				float cameraRightEdge = scrolling.x + (position.x * FG_sceneViewGridStep.x) + (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
				float cameraBottomEdge = scrolling.y + (-position.y * FG_sceneViewGridStep.y) + (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);

				Vector2 topLeftCorner = Vector2(cameraLeftEdge, cameraTopEdge);
				Vector2 bottomRightCorner = Vector2(cameraRightEdge, cameraBottomEdge);
				Vector2 topRightCorner = Vector2(cameraRightEdge, cameraTopEdge);
				Vector2 bottomLeftCorner = Vector2(cameraLeftEdge, cameraBottomEdge);

				float cameraTextureWidth = (float)FlatEngine::F_cameraIcon.GetWidth() / 4;
				float cameraTextureHeight = (float)FlatEngine::F_cameraIcon.GetHeight() / 4;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, FlatEngine::F_cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, FlatEngine::F_cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, FlatEngine::F_cameraBoxColor, 2.0f, draw_list);

				// Draw actual camera icon
				FlatEngine::AddImageToDrawList(FlatEngine::F_cameraIcon.GetTexture(), position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, step, draw_list, 0, IM_COL32(255, 255, 255, iconTransparency));
			}

			// Renders Canvas Component
			if (canvas != nullptr)
			{
				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float renderXStart = FG_sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x);
				float renderYStart = FG_sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x);
				Vector2 renderStart = Vector2(renderXStart, renderYStart);
				Vector2 renderEnd = Vector2(renderXStart + ((activeWidth * transformScale.x) * FG_sceneViewGridStep.x), renderYStart + ((activeHeight * transformScale.y) * FG_sceneViewGridStep.x));

				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 2);

				FlatEngine::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, FlatEngine::F_canvasBorderColor, 3.0f, draw_list);
			}

			// Renders Button Component
			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				float activeLeft = FG_sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * FG_sceneViewGridStep.x);
				float activeRight = FG_sceneViewCenter.x + ((position.x + (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * FG_sceneViewGridStep.x);
				float activeTop = FG_sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * FG_sceneViewGridStep.y);
				float activeBottom = FG_sceneViewCenter.y - ((position.y - (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * FG_sceneViewGridStep.y);

				Vector2 center = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };
				Vector2 topRight = { activeRight, activeTop };
				Vector2 bottomLeft = { activeLeft, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 2);

				if (rotation != 0)
				{
					float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
					float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

					topLeft = ImRotate(Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					topRight = ImRotate(Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					bottomRight = ImRotate(Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					bottomLeft = ImRotate(Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);

					Vector2 pos[4] =
					{
						Vector2(center.x + topLeft.x, center.y + topLeft.y),
						Vector2(center.x + topRight.x, center.y + topRight.y),
						Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
						Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
					};

					if (_isActive)
					{
						FlatEngine::DrawLine(pos[0], pos[1], FlatEngine::F_buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[1], pos[2], FlatEngine::F_buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[2], pos[3], FlatEngine::F_buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[3], pos[0], FlatEngine::F_buttonComponentActiveColor, 2.0f, draw_list);
					}
					else
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::F_buttonComponentInctiveColor, 1.0f, draw_list);
				}
				else
				{
					if (_isActive)
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::F_buttonComponentActiveColor, 1.0f, draw_list);
					else
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::F_buttonComponentInctiveColor, 1.0f, draw_list);
				}
			}

			// Renders BoxCollider Component
			for (BoxCollider *boxCollider : boxColliders)
			{
				float activeWidth = boxCollider->GetActiveWidth();
				float activeHeight = boxCollider->GetActiveHeight();
				Vector2 activeOffset = boxCollider->GetActiveOffset();
				int activeLayer = boxCollider->GetActiveLayer();
				bool _isActive = boxCollider->IsActive();
				bool _isColliding = boxCollider->IsColliding();
				float activeRadius = boxCollider->GetActiveRadiusScreen();
				bool _showActiveRadius = boxCollider->GetShowActiveRadius();
				Vector2 center = boxCollider->GetCenterCoord();

				boxCollider->UpdateActiveEdges(FG_sceneViewGridStep.x, FG_sceneViewCenter);

				Vector2 corners[4] = {
					boxCollider->GetCorners()[0],
					boxCollider->GetCorners()[1],
					boxCollider->GetCorners()[2],
					boxCollider->GetCorners()[3],
				};

				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 2);

				if (FlatEngine::F_LoadedProject.GetCollisionDetection() == "Shared Axis")
				{
					if (_isActive && !_isColliding)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderActiveColor, 1.0f, draw_list);
					else if (!_isActive)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderInactiveColor, 1.0f, draw_list);
					else if (_isColliding)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderCollidingColor, 1.0f, draw_list);
				}
				else if (FlatEngine::F_LoadedProject.GetCollisionDetection() == "Separating Axis")
				{
					Vector2 corners[4] = {
						boxCollider->GetCorners()[0],
						boxCollider->GetCorners()[1],
						boxCollider->GetCorners()[2],
						boxCollider->GetCorners()[3],
					};
					Vector2 normals[4] =
					{
						boxCollider->GetNormals()[0],
						boxCollider->GetNormals()[1],
						boxCollider->GetNormals()[2],
						boxCollider->GetNormals()[3],
					};

					// Draw Normals
					FlatEngine::DrawLine(center, normals[0], FlatEngine::F_colliderInactiveColor, 2.0f, draw_list);
					FlatEngine::DrawLine(center, normals[1], FlatEngine::F_colliderInactiveColor, 2.0f, draw_list);
					FlatEngine::DrawLine(center, normals[2], FlatEngine::F_colliderInactiveColor, 2.0f, draw_list);
					FlatEngine::DrawLine(center, normals[3], FlatEngine::F_colliderInactiveColor, 2.0f, draw_list);

					if (_isActive && !_isColliding)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderActiveColor, 1.0f, draw_list);
					else if (!_isActive)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderInactiveColor, 1.0f, draw_list);
					else if (_isColliding)
						FlatEngine::DrawRectangleFromLines(corners, FlatEngine::F_colliderCollidingColor, 1.0f, draw_list);
				}

				// Draw activeRadius circle
				if (_showActiveRadius)
					FlatEngine::DrawCircle(center, activeRadius, FlatEngine::F_colliderActiveColor, draw_list);
			}

			// Renders CircleCollider Component
			for (CircleCollider* circleCollider : circleColliders)
			{
				Vector2 activeOffset = circleCollider->GetActiveOffset();
				int activeLayer = circleCollider->GetActiveLayer();
				bool _isActive = circleCollider->IsActive();
				bool _isColliding = circleCollider->IsColliding();
				float activeRadius = circleCollider->GetActiveRadiusGrid() * FG_sceneViewGridStep.x;
				circleCollider->SetActiveRadiusScreen(activeRadius);
				bool _showActiveRadius = circleCollider->GetShowActiveRadius();
				Vector2 center = circleCollider->GetCenterCoord();

				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 2);

				circleCollider->UpdateActiveEdges(FlatEngine::F_LoadedProject.GetCollisionDetection(), FG_sceneViewGridStep.x, FG_sceneViewCenter);

				if (_isActive && !_isColliding)
					FlatEngine::DrawCircle(center, activeRadius, FlatEngine::F_colliderActiveColor, draw_list);
				else if (!_isActive)
					FlatEngine::DrawCircle(center, activeRadius, FlatEngine::F_colliderInactiveColor, draw_list);
				else if (_isColliding)
					FlatEngine::DrawCircle(center, activeRadius, FlatEngine::F_colliderCollidingColor, draw_list);
			}

			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most -- If this obect is focused
			if (focusedObjectID != -1 && focusedObjectID == self.GetID())
			{
				GameObject focusedObject = FlatEngine::GetObjectById(focusedObjectID);
				SDL_Texture* arrowToRender = FlatEngine::F_transformArrow.GetTexture();
				// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)FlatEngine::F_transformArrow.GetWidth() * 3;
				float arrowHeight = (float)FlatEngine::F_transformArrow.GetHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 3, arrowHeight - 3 };
				bool _scalesWithZoom = false;
				float transformMoveModifier = 0.02f;
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x + (position.x * step), FG_sceneViewCenter.y - (position.y * step));

				// Invisible button for Transform Arrow Move X and Y
				Vector2 moveAllStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
				FlatEngine::RenderInvisibleButton("TransformBaseArrowButton", moveAllStartPos, Vector2(28, 28), false);
				const bool _baseHovered = ImGui::IsItemHovered();
				const bool _baseActive = ImGui::IsItemActive();
				const bool _baseClicked = ImGui::IsItemClicked();

				if (_baseHovered || _baseActive)
				{
					arrowToRender = FlatEngine::F_transformArrowAllWhite.GetTexture();
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for X arrow
				Vector2 moveXStartPos = Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
				FlatEngine::RenderInvisibleButton("TransformBaseArrowXButton", moveXStartPos, Vector2(63, 35), false);
				const bool _xHovered = ImGui::IsItemHovered();
				const bool _xActive = ImGui::IsItemActive();
				const bool _xClicked = ImGui::IsItemClicked();
				if (_xHovered || _xActive)
				{
					arrowToRender = FlatEngine::F_transformArrowXWhite.GetTexture();
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for Y arrow
				Vector2 moveYStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
				FlatEngine::RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, Vector2(35, 63), false);
				const bool _yHovered = ImGui::IsItemHovered();
				const bool _yActive = ImGui::IsItemActive();
				const bool _yClicked = ImGui::IsItemClicked();
				if (_yHovered || _yActive)
				{
					arrowToRender = FlatEngine::F_transformArrowYWhite.GetTexture();
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				static Vector2 transformScreenPos = Vector2(0, 0);
				static Vector2 cursorPosAtClick = inputOutput.MousePos;
				Vector2 relativePosition = transform->GetPosition();

				if (_baseClicked || _xClicked || _yClicked)
				{
					cursorPosAtClick = inputOutput.MousePos;
					transformScreenPos = Vector2(origin.x + (relativePosition.x * step), origin.y - (relativePosition.y * step));
				}

				Vector2 transformPosOffsetFromMouse = Vector2((cursorPosAtClick.x - transformScreenPos.x) / step, (cursorPosAtClick.y - transformScreenPos.y) / step);
				Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - origin.x) / step, (origin.y - inputOutput.MousePos.y) / step);
				Vector2 newTransformPos = Vector2(mousePosInGrid.x - transformPosOffsetFromMouse.x, mousePosInGrid.y + transformPosOffsetFromMouse.y);				

				if (_baseActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(newTransformPos);
				else if (_xActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(Vector2(newTransformPos.x, relativePosition.y));
				else if (_yActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(Vector2(relativePosition.x, newTransformPos.y));


				// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow
				drawSplitter->SetCurrentChannel(draw_list, FlatEngine::F_maxSpriteLayers + 3);
				FlatEngine::AddImageToDrawList(arrowToRender, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, step, draw_list);
			}
		}
	}
}