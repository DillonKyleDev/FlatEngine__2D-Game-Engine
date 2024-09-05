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
#include "TileMap.h"
#include "TileSet.h"

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

namespace FL = FlatEngine;

/*
######################################
######							######
######    Gui Implimentation    ######
######                          ######
######################################
*/

using GameObject = FL::GameObject;
using Button = FL::Button;
using Component = FL::Component;
using MappingContext = FL::MappingContext;
using Transform = FL::Transform;
using Sprite = FL::Sprite;
using Camera = FL::Camera;
using Canvas = FL::Canvas;
using Text = FL::Text;
using BoxCollider = FL::BoxCollider;
using CircleCollider = FL::CircleCollider;
using Sound = FL::Sound;
using ComponentTypes = Component::ComponentTypes;
using TileMap = FL::TileMap;

namespace FlatGui 
{
	// For window styles
	ImDrawList* drawList = nullptr;

	// FlatGui Variables
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = nullptr;
	std::string FG_FocusedAnimationName = "";
	GameObject *objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	long previewAnimationStartTime = 0;
	long previewAnimationTime = 0;
	bool _playPreviewAnimation = false;
	long FocusedGameObjectID = -1;
	std::shared_ptr<GameObject> playerObject = nullptr;

	// TileMap
	std::string FG_currentSelectedColliderArea = "";
	std::vector<std::pair<Vector2, Vector2>> FG_collisionAreasBuffer = std::vector<std::pair<Vector2, Vector2>>();

	// Frame Counter
	int framesDrawn = 0;

	// Settings
	int iconTransparency = 50;

	// Hierarchy	
	std::map<long, bool> leafExpandedTracker = std::map<long, bool>();

	// Window Visibility
	bool FG_b_showDemoWindow = false;
	bool FG_b_showScriptEditor = false;
	bool FG_b_showTileSetEditor = false;
	bool FG_b_showFileExplorer = false;
	bool FG_b_showSceneView = true;
	bool FG_b_showGameView = true;
	bool FG_b_showHierarchy = true;
	bool FG_b_showInspector = true;
	bool FG_b_showAnimator = false;
	bool FG_b_showAnimationPreview = false;
	bool FG_b_showKeyFrameEditor = false;
	bool FG_b_showLogger = true;
	bool FG_b_showProfiler = true;
	bool FG_b_showMappingContextEditor = false;
	bool FG_b_showSettings = false;


	void Init()
	{
		FL::CreateNewScene();
	}

	void SetupProfilerProcesses()
	{
		if (FG_b_showProfiler)
		{
			// Add Profiler Processes
			// 						
			FL::AddProfilerProcess("Render");
			FL::AddProfilerProcess("Render Present");

			if (FG_b_showFileExplorer)
				FL::AddProfilerProcess("File Explorer");
			else
				FL::RemoveProfilerProcess("File Explorer");


			if (FG_b_showTileSetEditor)
				FL::AddProfilerProcess("TileSet Editor");
			else
				FL::RemoveProfilerProcess("TileSet Editor");

			if (FG_b_showScriptEditor)
				FL::AddProfilerProcess("Script Editor");
			else
				FL::RemoveProfilerProcess("Script Editor");

			if (FG_b_showHierarchy)
				FL::AddProfilerProcess("Hierarchy");
			else
				FL::RemoveProfilerProcess("Hierarchy");

			if (FG_b_showInspector)
				FL::AddProfilerProcess("Inspector");
			else
				FL::RemoveProfilerProcess("Inspector");

			if (FG_b_showGameView)
				FL::AddProfilerProcess("Game View");
			else
				FL::RemoveProfilerProcess("Game View");

			if (FG_b_showSceneView)
				FL::AddProfilerProcess("Scene View");
			else
				FL::RemoveProfilerProcess("Scene View");

			if (FG_b_showAnimator)
				FL::AddProfilerProcess("Animator");
			else
				FL::RemoveProfilerProcess("Animator");

			if (FG_b_showAnimationPreview)
				FL::AddProfilerProcess("Animation Preview");
			else
				FL::RemoveProfilerProcess("Animation Preview");

			if (FG_b_showKeyFrameEditor)
				FL::AddProfilerProcess("Key Frame Editor");
			else
				FL::RemoveProfilerProcess("Key Frame Editor");

			if (FG_b_showLogger)
				FL::AddProfilerProcess("Log");
			else
				FL::RemoveProfilerProcess("Log");

			if (FG_b_showProfiler)
				FL::AddProfilerProcess("Profiler");
			else
				FL::RemoveProfilerProcess("Profiler");

			if (FG_b_showMappingContextEditor)
				FL::AddProfilerProcess("Mapping Context Editor");
			else
				FL::RemoveProfilerProcess("Mapping Context Editor");
		
			if (FG_b_showSettings)
				FL::AddProfilerProcess("Settings");
			else
				FL::RemoveProfilerProcess("Settings");

			FL::AddProfilerProcess("Collision Testing");
		}
	}

	void RunOnceAfterInitialization()
	{
		FL::InitializeMappingContexts();
		//FL::InitializeTileSets();
		SetupProfilerProcesses();
	}


	void Cleanup()
	{
		// Remove Profiler Processes
		FL::RemoveProfilerProcess("Render");
		FL::RemoveProfilerProcess("Render Present");
	}

	void SetFocusedGameObjectID(long ID)
	{
		FocusedGameObjectID = ID;
		if (ID != -1)
		{
			GameObject *focusedObject = FL::GetObjectById(ID);
			Animation* animationComponent = focusedObject->GetAnimation();
			std::string animationPath = "";

			if (animationComponent != nullptr)
				animationPath = animationComponent->GetAnimationPath();

			// If applicable to the current animation, create a copy of the focused GameObject to be used for the animator window.
			if (FG_b_showAnimator && FocusedAnimation != nullptr &&
				animationComponent != nullptr && animationPath == FocusedAnimation->animationPath)
			{
				std::vector<GameObject> animatorObjects = std::vector<GameObject>();
				animatorObjects.clear();
				//objectForFocusedAnimation = GameObject(FL::GetObjectById(ID), animatorObjects, FL::GetLoadedScene()->GetSceneObjects(), -1);
				FL::Transform* transform = objectForFocusedAnimation->GetTransform();
				transform->SetPosition(Vector2(0, 0));
				animatorObjects.push_back(*objectForFocusedAnimation);
				//FL::GetLoadedScene()->SetAnimatorPreviewObjects(animatorObjects); // FIX LATER
			}
		}
	}

	long GetFocusedGameObjectID()
	{
		return FocusedGameObjectID;
	}

	void DestroySelf(std::shared_ptr<FL::GameObject> thisObject)
	{
		FL::DeleteGameObject(thisObject->GetID());
	}

	// Project Management
	void LoadProject(std::string path)
	{
		json projectJson;
		FL::LoadGameProject(path, projectJson);

		if (projectJson["Project Properties"][0] != "NULL")
		{
			// Loop through the saved Properties in the JSON file
			for (int i = 0; i < projectJson["Project Properties"].size(); i++)
			{
				// Get data from the loaded object
				json currentObjectJson = projectJson["Project Properties"][i];

				// Show/hide windows
				if (currentObjectJson.contains("_showSceneView"))
					FG_b_showSceneView = currentObjectJson["_showSceneView"];
				if (currentObjectJson.contains("_showGameView"))
					FG_b_showGameView = currentObjectJson["_showGameView"];
				if (currentObjectJson.contains("_showFileExplorer"))
					FG_b_showFileExplorer = currentObjectJson["_showFileExplorer"];
				if (currentObjectJson.contains("_showTileSetEditor"))
					FG_b_showTileSetEditor = currentObjectJson["_showTileSetEditor"];
				if (currentObjectJson.contains("_showScriptEditor"))
					FG_b_showScriptEditor = currentObjectJson["_showScriptEditor"];
				if (currentObjectJson.contains("_showHierarchy"))
					FG_b_showHierarchy = currentObjectJson["_showHierarchy"];
				if (currentObjectJson.contains("_showInspector"))
					FG_b_showInspector = currentObjectJson["_showInspector"];
				if (currentObjectJson.contains("_showAnimator"))
					FG_b_showAnimator = currentObjectJson["_showAnimator"];
				if (currentObjectJson.contains("_showAnimationPreview"))
					FG_b_showAnimationPreview = currentObjectJson["_showAnimationPreview"];
				if (currentObjectJson.contains("_showKeyFrameEditor"))
					FG_b_showKeyFrameEditor = currentObjectJson["_showKeyFrameEditor"];
				if (currentObjectJson.contains("_showLogger"))
					FG_b_showLogger = currentObjectJson["_showLogger"];
				if (currentObjectJson.contains("_showProfiler"))
					FG_b_showProfiler = currentObjectJson["_showProfiler"];
				if (currentObjectJson.contains("_showMappingContextEditor"))
					FG_b_showMappingContextEditor = currentObjectJson["_showMappingContextEditor"];

				// Current directory opened
				if (currentObjectJson.contains("currentFileDirectory"))
					FG_currentDirectory = currentObjectJson["currentFileDirectory"];

				// Settings
				if (currentObjectJson.contains("_clearLogBuffer"))
				{
					FG_b_clearBufferEveryFrame = currentObjectJson["_clearLogBuffer"];
					if (FG_b_clearBufferEveryFrame)
					{
						FL::F_Logger.ClearBuffer();
					}
				}
			}
		}

		if (FL::F_LoadedProject.GetLoadedPreviewAnimationPath() != "")
			SetFocusedAnimation(FL::LoadAnimationFile(FL::F_LoadedProject.GetLoadedPreviewAnimationPath()));
		Vector2 scrolling = FL::F_LoadedProject.GetSceneViewScrolling();
		FG_sceneViewScrolling = scrolling;
		Vector2 gridStep = FL::F_LoadedProject.GetSceneViewGridStep();
		FG_sceneViewGridStep = gridStep;

		if (FL::F_LoadedProject.GetFocusedGameObjectID() != -1 && FL::GetObjectById(FL::F_LoadedProject.GetFocusedGameObjectID()) != nullptr)
			SetFocusedGameObjectID(FL::F_LoadedProject.GetFocusedGameObjectID());
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
			{ "loadedAnimationPath", project.GetLoadedPreviewAnimationPath() },
			{ "sceneToLoadAtRuntime", project.GetRuntimeScene() },
			{ "buildPath", project.GetBuildPath() },
			{ "currentFileDirectory", FG_currentDirectory },
			{ "focusedGameObjectID", GetFocusedGameObjectID() },
			{ "sceneViewScrollingX", FG_sceneViewScrolling.x },
			{ "sceneViewScrollingY", FG_sceneViewScrolling.y },
			{ "sceneViewGridStepX", FG_sceneViewGridStep.x },
			{ "sceneViewGridStepY", FG_sceneViewGridStep.y },
			{ "_showFileExplorer", FG_b_showFileExplorer },
			{ "_showTileSetEditor", FG_b_showTileSetEditor },
			{ "_showScriptEditor", FG_b_showScriptEditor },
			{ "_showSceneView", FG_b_showSceneView },
			{ "_showGameView", FG_b_showGameView },
			{ "_showHierarchy", FG_b_showHierarchy },
			{ "_showInspector", FG_b_showInspector },
			{ "_showAnimator", FG_b_showAnimator },
			{ "_showAnimationPreview", FG_b_showAnimationPreview },
			{ "_showKeyFrameEditor", FG_b_showKeyFrameEditor },
			{ "_showLogger", FG_b_showLogger },
			{ "_showProfiler", FG_b_showProfiler },
			{ "_showMappingContextEditor", FG_b_showMappingContextEditor },
			{ "_clearLogBuffer", FG_b_clearBufferEveryFrame },
			{ "_autoSave", FL::F_LoadedProject.AutoSaveOn() },
			{ "resolutionWidth", FL::F_LoadedProject.GetResolution().x },
			{ "resolutionHeight", FL::F_LoadedProject.GetResolution().y },
			{ "_fullscreen", FL::F_LoadedProject.IsFullscreen() },
			{ "_vsyncEnabled", FL::F_LoadedProject.IsVsyncEnabled() },
			});
		projectProperties.push_back(animationName);

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"Project Properties", projectProperties } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SaveCurrentProject()
	{
		SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
	}

	std::vector<std::string> RetrieveProjectPaths()
	{
		std::vector<std::string> projectPaths = std::vector<std::string>();

		for (std::string projectPath : FL::FindAllFilesWithExtension(FL::GetDir("projectDir"), ".prj"))
		{
			projectPaths.push_back(projectPath);
		}

		return projectPaths;
	}

	void RenderProjectHub(bool& b_projectSelected, std::string &projectPath)
	{
		// Get all project files in the projects folder to present in the project selection screen
		static std::vector<std::string> projectPaths = RetrieveProjectPaths();

		bool b_isOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0,0));
		FL::SetNextViewportToFillWindow();  // Maximize viewport
		FL::BeginWindow("Project Hub", b_isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize, FL::GetColor("transparent"));
		// Set background to transparent
		ImGui::PushStyleColor(ImGuiCol_WindowBg, FL::GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("frameBg"));
		ImGui::PopStyleVar();

		// Get window dimensions for background image
		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();		
		Vector2 dimensions = Vector2((float)FL::GetTextureObject("selectProject")->GetWidth(), (float)FL::GetTextureObject("selectProject")->GetHeight());
		float headerHeight = dimensions.y;

		// Draw window background gradient
		ImGui::Image(FL::GetTexture("projectHubBg"), canvas_sz);

		// Reset cursor to before drawing the bg image
		ImGui::SetCursorScreenPos(canvas_p0);

		// Draw header background gradient
		ImGui::Image(FL::GetTexture("flatEngineLogoGradient"), Vector2(canvas_sz.x, headerHeight + 10));
		// Reset cursor to before the header bg image
		ImGui::SetCursorScreenPos(Vector2(canvas_p0.x + 10, canvas_p0.y + 5));

		// Draw the FL logo and header text image
		ImGui::Image(FL::GetTexture("flatEngineLogo"), Vector2(dimensions.y));
		ImGui::SameLine();
		ImGui::Image(FL::GetTexture("selectProject"), dimensions);

		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text("");
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 50, ImGui::GetCursorPosY()));
		ImGui::Text("Choose a project:");
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));

		Vector2 startProjects = ImGui::GetCursorScreenPos();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(50, 0));
		FL::BeginWindowChild("Projects", FL::GetColor("transparent"));
		ImGui::PopStyleVar();

		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, startProjects.y));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("projectSelectionTable"));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
		ImGui::BeginChild("ProjectsTable", Vector2(0, ImGui::GetContentRegionAvail().y - 100), FL::F_childFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		Vector2 startTable = ImGui::GetCursorScreenPos();

		ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::GetColor32("transparent"));
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::GetColor32("transparent"));
		ImGui::PushStyleColor(ImGuiCol_TableBorderLight, FL::GetColor32("transparent"));
		ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, FL::GetColor32("transparent"));
		FL::PushTable("#ProjectsTable", 1);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::TableNextRow();

		if (projectPaths.size() == 0)
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));
			ImGui::Text("Nothing to see here...");
		}
		else
		{
			for (std::string path : projectPaths)
			{
				ImGui::TableSetColumnIndex(0);
				ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
				if (FL::RenderButton(FL::GetFilenameFromPath(path), Vector2(ImGui::GetContentRegionAvail().x, 60), 1, FL::GetColor("projectHubButton"), FL::GetColor("projectHubButtonHovered"), FL::GetColor("projectHubButtonActive")))
				{
					b_projectSelected = true;
					projectPath = path;
				}
				ImGui::TableNextRow();
			}
		}
		float scrollY = ImGui::GetScrollY(); // Save table scroll for table outline
		FL::PopTable();
		FL::EndWindowChild();

		Vector2 endTable = ImGui::GetCursorScreenPos();

		// "New Project" button section
		ImGui::Text("");
		ImGui::Separator();

		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x - 110, ImGui::GetCursorScreenPos().y + 6));

		std::string projectName = "";
		static bool b_openProjectModal = false;

		if (FL::RenderButton("New Project", Vector2(100, 30)))
			b_openProjectModal = true;
		
		if (FL::RenderInputModal("Create New Project", "Project name", projectName, b_openProjectModal))
		{
			Project newProject = Project();
			std::string projectPath = FL::GetDir("projects") + "/" + projectName + ".prj";
			SaveProject(newProject, projectPath);
			LoadProject(projectPath);
			projectPaths = RetrieveProjectPaths();
		}

		FL::EndWindowChild();
				
		ImGui::GetWindowDrawList()->AddRect(Vector2(startTable.x - 6, startTable.y - 6 + scrollY), Vector2(startTable.x + ImGui::GetContentRegionAvail().x - 104, endTable.y - 3), FL::GetColor32("projectHubTableOutline"), 2);


		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		FL::EndWindow();
	}

	void AddViewports()
	{
		// ImGui Demo Window
		if (FG_b_showDemoWindow)
			ImGui::ShowDemoWindow(&FG_b_showDemoWindow);

		MainMenuBar();
		RenderToolbar();


		float startTime = (float)FL::GetEngineTime();

		if (FG_b_showScriptEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderScriptEditor();
			FL::AddProcessData("Script Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showTileSetEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderTileSetEditor();
			FL::AddProcessData("TileSet Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showFileExplorer)
		{
			startTime = (float)FL::GetEngineTime();
			RenderFileExplorer();
			FL::AddProcessData("File Explorer", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showHierarchy)
		{
			startTime = (float)FL::GetEngineTime();
			RenderHierarchy();
			FL::AddProcessData("Hierarchy", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showInspector)
		{
			startTime = (float)FL::GetEngineTime();
			RenderInspector();
			FL::AddProcessData("Inspector", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showGameView)
		{
			startTime = (float)FL::GetEngineTime();
			FL::Game_RenderView();
			FL::AddProcessData("Game View", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showSceneView)
		{
			startTime = (float)FL::GetEngineTime();
			Scene_RenderView();
			FL::AddProcessData("Scene View", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showAnimator)
		{
			startTime = (float)FL::GetEngineTime();
			RenderAnimator();
			FL::AddProcessData("Animator", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showAnimationPreview)
		{
			startTime = (float)FL::GetEngineTime();
			RenderAnimationPreview();
			FL::AddProcessData("Animation Preview", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showKeyFrameEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderKeyFrameEditor();
			FL::AddProcessData("Key Frame Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showLogger)
		{
			startTime = (float)FL::GetEngineTime();
			RenderLog();
			FL::AddProcessData("Log", (float)FL::GetEngineTime() - startTime);
		}
	
		if (FG_b_showProfiler)
		{
			startTime = (float)FL::GetEngineTime();
			RenderProfiler();
			FL::AddProcessData("Profiler", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showMappingContextEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderMappingContextEditor();
			FL::AddProcessData("Mapping Context Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showSettings)
		{
			startTime = (float)FL::GetEngineTime();
			RenderSettings();
			FL::AddProcessData("Settings", (float)FL::GetEngineTime() - startTime);
		}
	}


	// General in order to be used with multiple views that render objects (animation preview, scene view, etc)
	void RenderGridView(Vector2& centerPoint, Vector2 &scrolling, bool _weightedScroll, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, Vector2 &step, Vector2 centerOffset, bool b_showAxis)
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
			FL::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, drawList);
		// Draw vertical grid lines
		for (float y = trunc(fmodf(scrolling.y + canvas_p0.y, step.y)); y < canvas_p0.y + canvas_sz.y; y += step.y)
			FL::DrawLine(Vector2(canvas_p0.x, y), Vector2(canvas_p1.x, y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, drawList);

		// Draw our x and y axis blue and green lines
		//
		float divX = trunc(scrolling.x / step.x);
		float modX = fmodf(scrolling.x, step.x);
		float offsetX = (step.x * divX) + modX;
		float divY = trunc(scrolling.y / step.y);
		float modY = fmodf(scrolling.y, step.y);
		float offsetY = (step.y * divY) + modY;

		centerPoint = Vector2(offsetX + canvas_p0.x, offsetY + canvas_p0.y);

		if (b_showAxis)
		{
			// Blue, green and pink colors for axis and center
			Vector4 xColor = Vector4(0.2f, 0.2f, 0.9f, 1.0f);
			Vector4 yColor = Vector4(0, 0.82f, 0.14f, 1.0f);
			Vector4 centerColor = Vector4(1.0f, 0, 0.96f, 1.0f);

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
			FL::DrawLine(Vector2(drawYAxisAt, canvas_p0.y), Vector2(drawYAxisAt, canvas_p1.y), yColor, 1.0f, drawList);
			FL::DrawLine(Vector2(canvas_p0.x, drawXAxisAt), Vector2(canvas_p1.x, drawXAxisAt), xColor, 1.0f, drawList);
			FL::DrawPoint(Vector2(centerPoint.x, centerPoint.y), centerColor, drawList);
		}
	}

	void RenderViewObjects(std::vector<GameObject> objects, Vector2 centerPoint, Vector2 canvas_p0, Vector2 canvas_sz, float step)
	{
		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, FL::F_maxSpriteLayers + 5);

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
		TileMap* tileMap = self.GetTileMap();


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
				Vector2 spriteScaleFinal = spriteScale;
				// Get Input and Output
				ImGuiIO& inputOutput = ImGui::GetIO();

				Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x - canvas_p0.x + (position.x * step) - ((pivotOffset.x * FL::F_spriteScaleMultiplier * step) * scale.x * spriteScale.x), FG_sceneViewCenter.y - canvas_p0.y - (position.y * step - 20) - ((pivotOffset.y * FL::F_spriteScaleMultiplier * step) * scale.y * spriteScale.y));
				ImGui::SetCursorPos(positionOnScreen);
				//// This will catch our interactions  - 4096 for overlap or keyword if it works
				ImGui::SetNextItemAllowOverlap();

				if (!scale.x == 0)
					spriteScaleFinal.x *= scale.x;
				if (!scale.y == 0)
					spriteScaleFinal.y *= scale.y;

				ImGui::InvisibleButton(invisibleButtonID.c_str(), Vector2(spriteTextureWidth * FL::F_spriteScaleMultiplier * step * spriteScaleFinal.x, spriteTextureHeight * FL::F_spriteScaleMultiplier * step * spriteScaleFinal.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
				const bool is_hovered = ImGui::IsItemHovered(); // Hovered
				const bool is_active = ImGui::IsItemActive();   // Held
				const bool is_clicked = ImGui::IsItemClicked();

				if (is_clicked && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
				{
					SetFocusedGameObjectID(sprite->GetParentID());
				}

				// Show cursor position in scene view when pressing Alt
				if (is_hovered && inputOutput.KeyAlt)
				{
					RenderSceneViewTooltip();
				}

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
				if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
					drawSplitter->SetCurrentChannel(draw_list, renderOrder);
				else
					drawSplitter->SetCurrentChannel(draw_list, 0);

				// Draw the texture
				FL::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, pivotOffset, Vector2(transformScale.x * spriteScale.x, transformScale.y * spriteScale.y), _spriteScalesWithZoom, step, draw_list, rotation, ImGui::GetColorU32(tintColor));
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
					if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					FL::AddImageToDrawList(textTexture.GetTexture(), position, FG_sceneViewCenter, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, FG_sceneViewGridStep.x, draw_list, rotation);
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

				float cameraTextureWidth = (float)FL::GetTextureObject("camera")->GetWidth() / 4;
				float cameraTextureHeight = (float)FL::GetTextureObject("camera")->GetHeight() / 4;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FL::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, FL::GetColor("cameraBox"), 2.0f, draw_list);
				FL::DrawLine(topLeftCorner, bottomRightCorner, FL::GetColor("cameraBox"), 2.0f, draw_list);
				FL::DrawLine(topRightCorner, bottomLeftCorner, FL::GetColor("cameraBox"), 2.0f, draw_list);

				// Draw actual camera icon
				FL::AddImageToDrawList(FL::GetTexture("camera"), position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, step, draw_list, 0, IM_COL32(255, 255, 255, iconTransparency));
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

				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

				FL::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, FL::GetColor("canvasBox"), 3.0f, draw_list);
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

				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

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
						FL::DrawLine(pos[0], pos[1], FL::GetColor("buttonComponentActive"), 2.0f, draw_list);
						FL::DrawLine(pos[1], pos[2], FL::GetColor("buttonComponentActive"), 2.0f, draw_list);
						FL::DrawLine(pos[2], pos[3], FL::GetColor("buttonComponentActive"), 2.0f, draw_list);
						FL::DrawLine(pos[3], pos[0], FL::GetColor("buttonComponentActive"), 2.0f, draw_list);
					}
					else
						FL::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FL::GetColor("buttonComponentInactive"), 1.0f, draw_list);
				}
				else
				{
					if (_isActive)
						FL::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FL::GetColor("buttonComponentActive"), 1.0f, draw_list);
					else
						FL::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FL::GetColor("buttonComponentInactive"), 1.0f, draw_list);
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

				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

				if (transform->GetRotation() == 0)
				{
					if (_isActive && !_isColliding)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderActive"), 1.0f, draw_list);
					else if (!_isActive)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderInactive"), 1.0f, draw_list);
					else if (_isColliding)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderColliding"), 1.0f, draw_list);
				}
				else
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
					FL::DrawLine(center, normals[0], FL::GetColor("colliderInactive"), 2.0f, draw_list);
					FL::DrawLine(center, normals[1], FL::GetColor("colliderInactive"), 2.0f, draw_list);
					FL::DrawLine(center, normals[2], FL::GetColor("colliderInactive"), 2.0f, draw_list);
					FL::DrawLine(center, normals[3], FL::GetColor("colliderInactive"), 2.0f, draw_list);

					if (_isActive && !_isColliding)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderActive"), 1.0f, draw_list);
					else if (!_isActive)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderInactive"), 1.0f, draw_list);
					else if (_isColliding)
						FL::DrawRectangleFromLines(corners, FL::GetColor("colliderColliding"), 1.0f, draw_list);
				}

				// Draw activeRadius circle
				if (_showActiveRadius)
					FL::DrawCircle(center, activeRadius, FL::GetColor("colliderActive"), draw_list);
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

				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

				circleCollider->UpdateActiveEdges(FG_sceneViewGridStep.x, FG_sceneViewCenter);

				if (_isActive && !_isColliding)
					FL::DrawCircle(center, activeRadius, FL::GetColor("colliderActive"), draw_list);
				else if (!_isActive)
					FL::DrawCircle(center, activeRadius, FL::GetColor("colliderInactive"), draw_list);
				else if (_isColliding)
					FL::DrawCircle(center, activeRadius, FL::GetColor("colliderColliding"), draw_list);
			}

			// Renders TileMap Component
			if (tileMap != nullptr)
			{
				long id = tileMap->GetID();
				bool _isActive = tileMap->IsActive();
				float width = (float)tileMap->GetWidth();							// in tiles
				float height = (float)tileMap->GetHeight();							// in tiles
				float tileWidth = (float)tileMap->GetTileWidth();
				float tileHeight = (float)tileMap->GetTileHeight();
				float gridWidth = width * tileWidth / FL::F_pixelsPerGridSpace;		// in grid tiles
				float gridHeight = height * tileHeight / FL::F_pixelsPerGridSpace;	// in grid tiles
				int renderOrder = tileMap->GetRenderOrder();
				std::map<int, std::map<int, FL::Tile>> tiles = tileMap->GetTiles();

				static std::vector<Vector2> hoveredTiles = std::vector<Vector2>();
				static std::vector<Vector2> selectedTiles = std::vector<Vector2>();
				
		

				// For Drawing TileMap border and background color
				float renderXStart = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x);
				float renderYStart = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x);
				Vector2 renderStart = Vector2(renderXStart, renderYStart);
				Vector2 renderEnd = Vector2(renderXStart + ((gridWidth * transformScale.x) * FG_sceneViewGridStep.x), renderYStart + ((gridHeight * transformScale.y) * FG_sceneViewGridStep.x));
				Vector2 focusObjectButtonSize = Vector2(renderEnd.x - renderStart.x, renderEnd.y - renderStart.y);

				float tileWidthInPx = FG_sceneViewGridStep.x * (tileWidth / FL::F_pixelsPerGridSpace);
				float tileHeightInPx = FG_sceneViewGridStep.x * (tileHeight / FL::F_pixelsPerGridSpace);
				Vector2 tileSize = Vector2(tileWidthInPx, tileHeightInPx);

				// For selecting multiple tiles
				static Vector2 multiSelectStartTile = Vector2(-1, -1);
				static Vector2 multiSelectEndTile = Vector2(-1, -1);
				static Vector2 multiSelectCurrentHoveredTile = Vector2(-1, -1);
				static Vector2 savedMultiSelectStartTile = Vector2(-1, -1);

				// For Moving MultiSelected Tiles
				static Vector2 moveStartTile = Vector2(-1, -1);
				static Vector2 moveEndTile = Vector2(-1, -1);

				// For selecting Collision Area coordinates
				static std::vector<std::pair<Vector2, Vector2>> selectedCollisionCoords = std::vector<std::pair<Vector2, Vector2>>();
				static Vector2 colAreaStartTile = Vector2(-1, -1);
				static Vector2 colAreaEndTile = Vector2(-1, -1);

				// "Focus on this TileMap GameObject" button
				if (focusedObjectID != self.GetID() && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
				{
					if (focusObjectButtonSize.x <= 0 || focusObjectButtonSize.y <= 0)
					{
						focusObjectButtonSize = Vector2(1, 1);
					}
					std::string focusObjectButtonID = "##SelectThisTileMapObjectButton" + std::to_string(self.GetID()) + "-" + std::to_string(id);
					AddSceneViewMouseControls(focusObjectButtonID, renderStart, focusObjectButtonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);
					if (ImGui::IsItemClicked())
					{
						FL::LogString(self.GetName());
						SetFocusedGameObjectID(self.GetID());
					}
				}

				drawSplitter->SetCurrentChannel(draw_list, 0);

				// TileMap background color and border
				if (focusedObjectID == self.GetID())
				{
					ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgFocused"));
					FL::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, FL::GetColor("tileMapBoxFocused"), 2.0f, draw_list);
				}
				else
				{
					ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgUnfocused"));
					FL::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, FL::GetColor("tileMapBoxUnfocused"), 2.0f, draw_list);
				}

				if (focusedObjectID == self.GetID())
				{
					// Handle Tiles
					for (float w = 0; w < width; w++)
					{
						for (float h = 0; h < height; h++)
						{
							// TileMap interactions
							//
							std::string tileButtonID = "##tileMapIndexButton" + std::to_string(id) + "-" + std::to_string(w) + std::to_string(h);
							TileSet* activeTileSet = nullptr;

							// Get active TileSet for texture dimensions
							std::string activeTileSetName = FL::F_tileSetAndIndexOnBrush.first;
							if (activeTileSetName != "")
							{
								activeTileSet = FL::GetTileSet(activeTileSetName);
							}

							// tileStart = viewport center + top left corner in pixel screen space + tile offset
							float tileStartX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (w * tileWidthInPx);
							float tileStartY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x) + (h * tileHeightInPx);

							Vector2 tileStart = Vector2(tileStartX, tileStartY);
							Vector2 tileEnd = Vector2(tileStartX + tileWidthInPx, tileStartY + tileHeightInPx);

							// Catch interactions on the TileMap container
							if ((focusedObjectID == self.GetID()) &&
								((FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH && activeTileSet != nullptr && FL::F_tileSetAndIndexOnBrush.second != -1) ||
								(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE) ||
								(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW && tileMap->GetSelectedCollisionArea() != "")) ||
								(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) ||
								(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
							{
								// Set Draw Channel to 2 for lower level UI
								drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 2);

								AddSceneViewMouseControls(tileButtonID, tileStart, tileSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("tileMapGridLines"));
								// _RectOnly flag enables the buttons to work when dragging the mouse over them in a clicked state // https://github.com/ocornut/imgui/commit/564ff2dfd379d40568879a5bc89e8cfea7e51d2f
								const bool is_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);



								// Collect all boxes between multiSelectStartTile tile and multiSelectCurrentlyHovered tile
								if (focusedObjectID == self.GetID() &&
									multiSelectStartTile.x != -1 &&
									multiSelectStartTile.y != -1 &&
									(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) &&
									ImGui::IsKeyDown(ImGuiKey_MouseLeft) &&
									(multiSelectStartTile.x <= w && multiSelectCurrentHoveredTile.x >= w || multiSelectCurrentHoveredTile.x <= w && multiSelectStartTile.x >= w) &&
									(multiSelectStartTile.y <= h && multiSelectCurrentHoveredTile.y >= h || multiSelectCurrentHoveredTile.y <= h && multiSelectStartTile.y >= h))
								{
									if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
									{
										ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
									}
									else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
									{
										ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileMultiSelectHighlightDragging"));

										bool b_containsTile = false;
										for (Vector2 tile : hoveredTiles)
										{
											if (tile.x == w && tile.y == h)
												b_containsTile = true;
										}
										if (!b_containsTile)
										{
											hoveredTiles.push_back(Vector2(w, h));
										}
									}
								}

								if (is_hovered)
								{
									// Mouse down
									if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
									{
										// Record the tile that is currently under the clicked mouse
										multiSelectCurrentHoveredTile = Vector2(w, h);

										if (activeTileSet != nullptr && FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoldingTile"));
											tileMap->SetTile(Vector2(w, h), activeTileSet, FL::F_tileSetAndIndexOnBrush.second);
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClick"));
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClickBorder"));
											tileMap->EraseTile(Vector2(w, h));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
										{
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSelectModeClickBorder"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
										{
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileMoveModeClickBorder"));
										}
									}
									// Mouse not down
									else
									{
										if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHover"));
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHoverBorder"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHoveredHighlight"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
											ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
										}
									}

									// Click pressed this frame
									if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false))
									{
										if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
										{
											// Record the tile that was clicked on	
											multiSelectStartTile = Vector2(w, h);
											savedMultiSelectStartTile = Vector2(w, h);
										}

										if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
										{
											colAreaStartTile = Vector2(w, h);
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
										{
											hoveredTiles.clear();
											selectedTiles.clear();
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
										{
											moveStartTile = Vector2(w, h);
										}
									}

									// Click released this frame
									if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
									{
										if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
										{
											// Record the tile that the mouse released on
											multiSelectEndTile = Vector2(w, h);
										}


										if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
										{
											colAreaEndTile = Vector2(w, h);

											if (colAreaStartTile.x != -1 && colAreaStartTile.y != -1)
											{
												std::pair<Vector2, Vector2> newPair = { colAreaStartTile, colAreaEndTile };
												bool b_alreadyContains = false;

												for (std::vector<std::pair<Vector2, Vector2>>::iterator coordPair = FG_collisionAreasBuffer.begin(); coordPair != FG_collisionAreasBuffer.end(); coordPair++)
												{
													if ((coordPair->first == colAreaStartTile && coordPair->second == colAreaEndTile) || (coordPair->first == colAreaEndTile && coordPair->second == colAreaStartTile))
													{
														FG_collisionAreasBuffer.erase(coordPair);
														b_alreadyContains = true;
														break;
													}
												}

												if (!b_alreadyContains)
												{
													FG_collisionAreasBuffer.push_back(newPair);
												}
											}

											colAreaStartTile = Vector2(-1, -1);
											colAreaEndTile = Vector2(-1, -1);
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
										{
											selectedTiles = hoveredTiles;
											hoveredTiles.clear();
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
										{
											moveEndTile = Vector2(w, h);

											if (moveStartTile.x != -1 && moveStartTile.y != -1)
											{
												float xMove = moveEndTile.x - moveStartTile.x;
												float yMove = moveEndTile.y - moveStartTile.y;

												tileMap->MoveTiles(selectedTiles, Vector2(xMove, yMove));
											}

											moveStartTile = Vector2(-1, -1);
											moveEndTile = Vector2(-1, -1);
										}

										if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
										{
											// Reset the starting tile
											multiSelectStartTile = Vector2(-1, -1);
										}
									}
								}
							}
						}
					}

					// Draw box around selected multiselect tiles
					if ((selectedTiles.size() > 0) && (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
					{
						float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (savedMultiSelectStartTile.x * tileWidthInPx);
						float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.y) + (savedMultiSelectStartTile.y * tileHeightInPx);
						float selectWidth = multiSelectEndTile.x - savedMultiSelectStartTile.x;
						if (selectWidth < 0)
							selectWidth *= -1;
						selectWidth += 1;
						float selectHeight = multiSelectEndTile.y - savedMultiSelectStartTile.y;
						if (selectHeight < 0)
							selectHeight *= -1;
						selectHeight += 1;

						Vector2 startTileScreenPos = Vector2(startPosX, startPosY);
						Vector2 endTileScreenPos = Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

						ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
						ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

						if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
						{
							FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
							}
						}
					}
				}

				// Draw box around each of the collision areas in the buffer
				if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
				{
					for (std::pair<Vector2, Vector2> collAreaBuffer : FG_collisionAreasBuffer)
					{
						Vector2 startCoord = collAreaBuffer.first;
						Vector2 endCoord = collAreaBuffer.second;

						float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (startCoord.x * tileWidthInPx);
						float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.y) + (startCoord.y * tileHeightInPx);
						float selectWidth = endCoord.x - startCoord.x;
						if (selectWidth < 0)
							selectWidth *= -1;
						selectWidth += 1;
						float selectHeight = endCoord.y - startCoord.y;
						if (selectHeight < 0)
							selectHeight *= -1;
						selectHeight += 1;

						Vector2 startTileScreenPos = Vector2(startPosX, startPosY);
						Vector2 endTileScreenPos = Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

						ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
						ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

						if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
						{
							FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
							}
						}
					}
				}

				// Draw TileMap indice Textures
				for (int w = 0; w < width; w++)
				{
					if (tiles.count((int)w) > 0)
					{
						for (int h = 0; h < height; h++)
						{
							if (tiles.at((int)w).count((int)h) > 0)
							{
								FL::Tile tile = tiles.at((int)w).at((int)h);

								// Get TileSet for this tiles texture data
								TileSet* usedTileSet = nullptr;
								std::string tileSetName = tile.tileSetName;
								if (tileSetName != "")
									usedTileSet = FL::GetTileSet(tileSetName);

								// this many grid spaces fit into a single tiles width (if tileWidth is 16: 16 / 8 is 2 grid spaces in for a single tile 
								float gridWidthsInATile = tileWidth / FL::F_pixelsPerGridSpace;
								float gridHeightsInATile = tileHeight / FL::F_pixelsPerGridSpace;

								SDL_Texture* texture = tile.tileSetTexture;
								float textureWidth = (float)usedTileSet->GetTexture()->GetWidth();
								float textureHeight = (float)usedTileSet->GetTexture()->GetHeight();
								Vector2 uvStart = Vector2(tile.uvStart.x / textureWidth, tile.uvStart.y / textureHeight);
								Vector2 uvEnd = Vector2(tile.uvEnd.x / textureWidth, tile.uvEnd.y / textureHeight);
								float gridXPosition = (position.x - (gridWidth / 2)) + gridWidthsInATile * w;
								float gridYPosition = (position.y + (gridHeight / 2)) - gridHeightsInATile * h;
								Vector2 tilePosition = Vector2(gridXPosition, gridYPosition);

								//////////////////

								// Change the draw channel for the scene object
								if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
									drawSplitter->SetCurrentChannel(draw_list, renderOrder);
								else
									drawSplitter->SetCurrentChannel(draw_list, 0);

								FL::AddImageToDrawList(texture, tilePosition, FG_sceneViewCenter, tileWidth, tileHeight, Vector2(0, 0), scale, true, FG_sceneViewGridStep.x, draw_list, 0, FL::GetColor32("white"), uvStart, uvEnd);
							}
						}
					}
				}
			}

			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most -- If this obect is focused
			if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE && focusedObjectID != -1 && focusedObjectID == self.GetID())
			{
				GameObject *focusedObject = FL::GetObjectById(focusedObjectID);
				SDL_Texture* arrowToRender = FL::GetTexture("transformArrow");
				// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)FL::GetTextureObject("transformArrow")->GetWidth() * 3;
				float arrowHeight = (float)FL::GetTextureObject("transformArrow")->GetHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 3, arrowHeight - 3 };
				bool _scalesWithZoom = false;
				float transformMoveModifier = 0.02f;
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x + (position.x * step), FG_sceneViewCenter.y - (position.y * step));
			
				// Invisible button for Transform Arrow Move X and Y
				Vector2 moveAllStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
				FL::RenderInvisibleButton("##TransformBaseArrowButton", moveAllStartPos, Vector2(28, 28), false);
				const bool _baseHovered = ImGui::IsItemHovered();
				const bool _baseActive = ImGui::IsItemActive();
				const bool _baseClicked = ImGui::IsItemClicked();

				if (_baseHovered || _baseActive)
				{
					arrowToRender = FL::GetTexture("transformArrowAllWhite");
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for X arrow
				Vector2 moveXStartPos = Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
				FL::RenderInvisibleButton("##TransformBaseArrowXButton", moveXStartPos, Vector2(63, 35), false);
				const bool _xHovered = ImGui::IsItemHovered();
				const bool _xActive = ImGui::IsItemActive();
				const bool _xClicked = ImGui::IsItemClicked();
				if (_xHovered || _xActive)
				{
					arrowToRender = FL::GetTexture("transformArrowXWhite");
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for Y arrow
				Vector2 moveYStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
				FL::RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, Vector2(35, 63), false);
				const bool _yHovered = ImGui::IsItemHovered();
				const bool _yActive = ImGui::IsItemActive();
				const bool _yClicked = ImGui::IsItemClicked();
				if (_yHovered || _yActive)
				{
					arrowToRender = FL::GetTexture("transformArrowYWhite");
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
				drawSplitter->SetCurrentChannel(draw_list, FL::F_maxSpriteLayers + 3);
				FL::AddImageToDrawList(arrowToRender, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, step, draw_list);
			}
		}
	}

	void AddSceneViewMouseControls(std::string buttonID, Vector2 startPos, Vector2 size, Vector2 &scrolling, Vector2 centerPoint, Vector2 &gridStep, Uint32 rectColor, bool b_filled, ImGuiButtonFlags buttonFlags, bool b_allowOverlap)
	{
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();
		bool _weightedScroll = true;
		Vector2 endPos = Vector2(startPos.x + size.x, startPos.y + size.y);

		// For calculating scrolling mouse position and what vector to zoom to
		DYNAMIC_VIEWPORT_WIDTH = trunc(endPos.x - startPos.x);
		DYNAMIC_VIEWPORT_HEIGHT = trunc(endPos.y - startPos.y);

		// Border
		if (b_filled)
			ImGui::GetWindowDrawList()->AddRectFilled(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
		else
			ImGui::GetWindowDrawList()->AddRect(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);

		//ImGui::SetNextItemAllowOverlap();
		FL::RenderInvisibleButton(buttonID.c_str(), startPos, size, b_allowOverlap, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held
		const bool is_clicked = ImGui::IsItemClicked();

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{			
			// This does not seem to work properly when resizing the window
			// inputOutput.MousePos and MouseDelta give incorrect values after upon dragging the mouse
			scrolling.x += inputOutput.MouseDelta.x;
			scrolling.y += inputOutput.MouseDelta.y;

			SaveCurrentProject();
		}

		// Get scroll amount for changing zoom level of scene view
		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.01f;
		float signedMousePosX = mousePos.x - centerPoint.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
		float signedMousePosY = mousePos.y - centerPoint.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
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
					scrolling.x -= trunc(signedMousePosX * weight);
					scrolling.y -= trunc(signedMousePosY * weight);
				}
				gridStep.x += finalZoomSpeed;
				gridStep.y += finalZoomSpeed;
			}
			else if (scrollInput < 0 && gridStep.x > 2 && gridStep.y > 2)
			{
				if (_weightedScroll)
				{
					scrolling.x += trunc(signedMousePosX * weight);
					scrolling.y += trunc(signedMousePosY * weight);
				}
				gridStep.x -= finalZoomSpeed;
				gridStep.y -= finalZoomSpeed;
			}
		}
	}
}