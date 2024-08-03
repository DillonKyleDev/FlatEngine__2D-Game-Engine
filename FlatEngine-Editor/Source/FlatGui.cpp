#include "Vector4.h"
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
#include "ScriptComponent.h"
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
#include "ImSequencer.h"
// For getting directory name
#include <windows.h>


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
	float childPadding = 8;
	ImDrawList* drawList = nullptr;

	// Managers
	WidgetsManager widgetsManager = WidgetsManager();

	// FlatGui Variables
	std::shared_ptr<Project> loadedProject = nullptr;
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = nullptr;
	GameObject objectForFocusedAnimation = GameObject(nullptr);
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	int previewAnimationStartTime = 0;
	int previewAnimationTime = 0;
	bool _playPreviewAnimation = false;
	long FocusedGameObjectID = -1;
	std::shared_ptr<GameObject> playerObject = nullptr;


	//////////////////////
	//Global      Colors//
	//  //////VV///////  /
	//     UUUUUUUU     //
	///\-------------/////
	//////////////////////
	Vector4 transparentColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 whiteColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//////////////////////
	// Components ////////
	//////////////////////
	Vector4 componentBorderColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Colliders
	Vector4 colliderActiveColor = Vector4(0.19f, 0.9f, 0.2f, 0.5f);
	Vector4 colliderInactiveColor = Vector4(0.9f, 0.2f, 0.2f, 0.5f);
	Vector4 colliderCollidingColor = Vector4(0.76f, 0.42f, 0.0f, 0.5f);
	// Button Components
	Vector4 buttonComponentActiveColor = Vector4(0.19f, 0.9f, 0.2f, 0.5f);
	Vector4 buttonComponentInctiveColor = Vector4(0.9f, 0.2f, 0.2f, 0.5f);
	// Canvas Orange
	Vector4 canvasBorderColor = Vector4(0.76f, 0.42f, 0.0f, 0.5f);
	// Camera
	Vector4 cameraBoxColor = Vector4(1.0f, 0.11f, 0.11f, 0.27f);
	//////////////////////
	// Windows ///////////
	//////////////////////
	Vector4 windowBgColor = Vector4(0.08f, 0.08f, 0.10f, 1.0f);
	Vector4 outerWindowColor = Vector4(0.13f, 0.13f, 0.15f, 1.0f);
	Vector4 innerWindowColor = Vector4(0.1f, 0.1f, 0.12f, 1.0f);
	Vector4 singleItemColor = Vector4(0.16f, 0.16f, 0.17f, 1.0f);
	Vector4 singleItemDark = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 windowTitleBg = Vector4(0.25f, 0.25f, 0.25f, 1.0f);
	// Log
	Vector4 logTextColor = Vector4(0.75f, 0.75f, 0.75f, 1.0f);
	Vector4 logBgColor = Vector4(0.2f, 0.2f, 0.22f, 1.0f);
	Vector4 logOutlineColor = Vector4(0.25f, 0.25f, 0.27f, 1.0f);
	// Docking
	Vector4 dockingPreviewColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 dockingPreviewEmptyColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	// ImGui Key colors
	Vector4 frameBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 frameBgActiveColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 frameBgHoveredColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// tabs
	Vector4 tabColor = Vector4(0.15f, 0.15f, 0.2f, 1.0f);
	Vector4 tabActiveColor = Vector4(0.4f, 0.4f, 0.42f, 1.0f);
	Vector4 tabHoveredColor = Vector4(0.4f, 0.4f, 0.42f, 1.0f);
	Vector4 tabUnfocusedColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4 tabUnfocusedActiveColor = Vector4(0.3f, 0.3f, 0.31f, 1.0f);
	// titles
	Vector4 titleBgColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 titleBgActiveColor = Vector4(0.2f, 0.2f, 0.25f, 1.0f);
	Vector4 titleBgCollapsedColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Text
	Vector4 textSelectedBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Resizers
	Vector4 resizeGripColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 resizeGripHoveredColor = Vector4(0.35f, 0.35f, 0.75f, 1.0f);
	Vector4 resizeGripActiveColor = Vector4(0.2f, 0.2f, 0.5f, 0.8f);
	// Misc (not sure what they're for)
	Vector4 popupBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 navWindowHighlightColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 navHighlightColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 navWindowDimBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 modalWindowDimBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	////////////////////
	// Custom Colors ///
	//////////////////// 
	// Inputs
	Vector4 inputColor = Vector4(0.3f, 0.3f, 0.32f, 1.0f);
	// Tables
	Vector4 noEditTableTextColor = Vector4(0.75f, 0.75f, 0.81f, 1.0f);
	Vector4 noEditTableRowFieldBgColor = Vector4(0.1f, 0.1f, 0.5f, 0.2f);// Vector4(0.3f, 0.35f, 0.55f, 1.0f);// Vector4(0.2f, 0.25f, 0.45f, 1.0f);
	Vector4 noEditTableRowValueBgColor = Vector4(0.1f, 0.1f, 0.5f, 0.2f);// Vector4(0.3f, 0.3f, 0.7f, 0.2f);
	Vector4 tableCellLightColor = Vector4(0.19f, 0.19f, 0.21f, 1.0f);
	Vector4 tableCellDarkColor = Vector4(0.24f, 0.24f, 0.27f, 1.0f);
	// Trees
	Vector4 treeSelectableColor = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	Vector4 treeSelectableHoveredColor = Vector4(0.3f, 0.35f, 0.65f, 1.0f);
	Vector4 treeSelectableActiveColor = Vector4(0.2f, 0.25f, 0.45f, 1.0f);
	Vector4 treeSelectableSelectedColor = Vector4(0.5f, 0.5f, 0.8f, 1.0f);
	Vector4 hierarchyChildObjectColor = Vector4(0.3f, 0.3f, 0.7f, 0.2f);
	// Combos
	Vector4 comboBgColor = Vector4(0.19f, 0.19f, 0.21f, 1.0f);
	Vector4 comboHoveredColor = Vector4(0.25f, 0.25f, 0.26f, 1.0f);
	Vector4 comboSelectableColor = Vector4(0.34f, .34f, .4f, 1.0f);
	Vector4 comboSelectedColor = Vector4(0.45f, 0.45f, 0.50f, 1.0f);
	Vector4 comboHighlightedColor = Vector4(0.25f, 0.25f, 0.28f, 1.0f);
	Vector4 comboArrowColor = Vector4(0.11f, 0.11f, 0.13f, 1.0f);
	Vector4 comboArrowHoveredColor = Vector4(0.15f, 0.15f, 0.16f, 1.0f);
	// Buttons
	Vector4 buttonColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 buttonHoveredColor = Vector4(0.35f, 0.35f, 0.75f, 1.0f);
	Vector4 buttonActiveColor = Vector4(0.2f, 0.2f, 0.5f, 0.8f);
	Vector4 imageButtonColor = Vector4(0.18f, 0.18f, 0.18f, 1.0f);
	Vector4 imageButtonDarkColor = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	Vector4 imageButtonHoveredColor = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	Vector4 imageButtonActiveColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4 imageButtonTintColor = whiteColor;
	// Sliders/Drags
	Vector4 sliderColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 sliderHoveredColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 sliderActiveColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 dragColor = Vector4(0.2f, 0.2f, 0.2f, 0.0f);
	Vector4 dragHoveredColor = Vector4(0.45f, 0.45f, 0.45f, 1.0f);
	Vector4 dragActiveColor = Vector4(0.10f, 0.10f, 0.10f, 1.0f);
	// Checkboxes
	Vector4 checkboxBgColor = Vector4(0.28f, 0.28f, 0.29f, 1.0f);
	Vector4 checkboxCheckColor = Vector4(0.45f, 0.45f, 0.9f, 1.0f);
	Vector4 checkboxHoveredColor = Vector4(0.31f, 0.31f, 0.32f, 1.0f);
	Vector4 checkboxActiveColor = Vector4(0.15f, 0.15f, 0.23f, 1.0f);
	////////////////////
	//// End Colors ////
	////////////////////

	// Flags
	ImGuiChildFlags autoResizeChildFlags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags resizeChildFlags = ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags childFlags = ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags headerFlags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;
	//ImGuiTableFlags_Resizable
	ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
	ImGuiTableFlags resizeableTableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
	ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

	// For rendering sprites
	int maxSpriteLayers = 55;
	float spriteScaleMultiplier = 0.2f;

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


	void SetupImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls for imgui ui nav
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowPadding = { 5.0f, 5.0f };
		style.DockingSeparatorSize = 1;
		style.SeparatorTextAlign = Vector2(0.5f, 0.0f);
		style.SeparatorTextBorderSize = 1;

		ImGui_ImplSDL2_InitForSDLRenderer(Window::window, Window::renderer);
		ImGui_ImplSDLRenderer2_Init(Window::renderer);


		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Load in what is currently in SavedScenes.json
			//FlatEngine::LoadScene("Level1.json");

			// Remove the reference to the imgui.ini file for layout since we only need that in Engine mode and
			// we don't want to have to include it in the final release build anyway.
			io.IniFilename = NULL;

			// Set fullscreen here for now
			Window::SetFullscreen(loadedProject->IsFullscreen());
		}
		else
			FlatEngine::CreateNewScene();

		// Round about way of editing the active titlebgactive color since pushstylecolor doesn't seem to work for it.
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName(i);
			//LogString(name);
			if (name == "TitleBgActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = titleBgActiveColor;
			}
			if (name == "TabUnfocusedActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = tabUnfocusedActiveColor;
			}
			if (name == "TabActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = tabActiveColor;
			}
			if (name == "ResizeGripHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = resizeGripHoveredColor;
			}
			if (name == "ResizeGripActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = resizeGripActiveColor;
			}
			if (name == "DockingPreview")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = dockingPreviewColor;
			}
			if (name == "DockingEmptyBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = dockingPreviewEmptyColor;
			}
		}
	}

	void SetupProfilerProcesses()
	{
		if (_showProfiler && FlatEngine::_isDebugMode)
		{
			// Add Profiler Processes
			// 						
			AddProfilerProcess("Render");
			AddProfilerProcess("Render Present");
			// RenderMainMenuBar()						
			//AddProfilerProcess("Render Main Menu Bar");
			// RenderToolbar()							
			//AddProfilerProcess("Render Toolbar");

			// RenderHierarchy()
			if (_showHierarchy)
				AddProfilerProcess("Render Hierarchy");
			else
				RemoveProfilerProcess("Render Hierarchy");

			// RenderInspector()
			if (_showInspector)
				AddProfilerProcess("Render Inspector");
			else
				RemoveProfilerProcess("Render Inspector");

			// Game_RenderView
			if (_showGameView)
				AddProfilerProcess("Render Game View");
			else
				RemoveProfilerProcess("Render Game View");

			// Scene_RenderView
			if (_showSceneView)
				AddProfilerProcess("Render Scene View");
			else
				RemoveProfilerProcess("Render Scene View");

			// RenderAnimator
			if (_showAnimator)
				AddProfilerProcess("Render Animator");
			else
				RemoveProfilerProcess("Render Animator");

			// RenderAnimationPreview
			if (_showAnimationPreview)
				AddProfilerProcess("Render Animation Preview");
			else
				RemoveProfilerProcess("Render Animation Preview");

			// RenderKeyFrameEditor
			if (_showKeyFrameEditor)
				AddProfilerProcess("Render Key Frame Editor");
			else
				RemoveProfilerProcess("Render Key Frame Editor");

			// RenderLog
			if (_showLogger)
				AddProfilerProcess("Render Log");
			else
				RemoveProfilerProcess("Render Log");

			// RenderProfiler
			if (_showProfiler)
				AddProfilerProcess("Render Profiler");
			else
				RemoveProfilerProcess("Render Profiler");

			// RenderMappingContextEditor
			if (_showMappingContextEditor)
				AddProfilerProcess("Render Mapping Context Editor");
			else
				RemoveProfilerProcess("Render Mapping Context Editor");

			// RenderSettings
			if (_showSettings)
				AddProfilerProcess("Render Settings");
			else
				RemoveProfilerProcess("Render Settings");
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

			// Open Project by default
			OpenProject("C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\WindowsApplication\\projects\\Sandbox.json");

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

	void Render(bool& quit)
	{
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Create dockable background space for all viewports
		ImGui::DockSpaceOverViewport();

		//Add viewport(s)
		// 
		// If Release
		if (!FlatEngine::_isDebugMode)
		{
			// Just Add GameView		
			Game_RenderView();
		}
		// Else add FlatEngine viewports
		else
			AddViewports();
	}

	void RenderClear()
	{
		// Rendering
		Vector4 clear_color = Vector4(1.00f, 1.00f, 1.00f, 1.00f);
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		SDL_RenderSetScale(Window::renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(Window::renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(Window::renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		
		// TO DO 
		// When vsync is off, find a way to only call renderpresent once per refresh depending on the rate of the screen
		Uint32 renderPresentStart = FlatEngine::GetEngineTime(); // Profiler
		SDL_RenderPresent(Window::renderer);
		AddProcessData("Render Present", (float)FlatEngine::GetEngineTime() - renderPresentStart); // Profiler

		// For things we only want to execute once after complete initialization
		RunOnceAfterInitialization();
	}

	void Cleanup()
	{
		// Remove Profiler Processes
		if (FlatEngine::_isDebugMode)
		{
			RemoveProfilerProcess("Render");
			RemoveProfilerProcess("Render Present");
		}

		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void HandleEvents(bool &quit)
	{
		std::vector<std::shared_ptr<FlatEngine::MappingContext>> mappingContexts = FlatEngine::mappingContexts;
		using XInputAxis = FlatEngine::XInputAxis;
		using XInputButtons = FlatEngine::XInputButtons;
		using XInputHats = FlatEngine::XInputHats;
		int JOYSTICK_DEAD_ZONE = FlatEngine::JOYSTICK_DEAD_ZONE;

		// Unfire all keybinds that were fired in the last frame then clear the saved keys
		static std::vector<std::string> firedKeys = std::vector<std::string>();
		for (std::string keybind : firedKeys)
			for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
				mappingContext->UnFireEvent(keybind);
		firedKeys.clear();


		SDL_Event event;
		//if (SDL_PollEvent(&event))
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				quit = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
				quit = true;
			// Keyboard Keys Down
			if (event.type == SDL_KEYDOWN)
			{
				// Send event to context inputAction
				switch (event.key.keysym.sym)
				{
				case SDLK_SPACE:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_SPACE") != "" && mappingContext->GetKeyBoundEvent("SDLK_SPACE").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_SPACE", event);
							firedKeys.push_back("SDLK_SPACE");
						}
					break;

				case SDLK_UP:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_UP") != "" && mappingContext->GetKeyBoundEvent("SDLK_UP").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_UP", event);
							firedKeys.push_back("SDLK_UP");
						}
					break;

				case SDLK_DOWN:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_DOWN") != "" && mappingContext->GetKeyBoundEvent("SDLK_DOWN").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_DOWN", event);
							firedKeys.push_back("SDLK_DOWN");
						}
					break;

				case SDLK_LEFT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_LEFT") != "" && mappingContext->GetKeyBoundEvent("SDLK_LEFT").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_LEFT", event);
							firedKeys.push_back("SDLK_LEFT");
						}
					break;

				case SDLK_RIGHT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_RIGHT") != "" && mappingContext->GetKeyBoundEvent("SDLK_RIGHT").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_RIGHT", event);
							firedKeys.push_back("SDLK_RIGHT");
						}
					break;

				case SDLK_a:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_a") != "" && mappingContext->GetKeyBoundEvent("SDLK_a").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_a", event);
							firedKeys.push_back("SDLK_a");
						}
					break;

				case SDLK_b:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_b") != "" && mappingContext->GetKeyBoundEvent("SDLK_b").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_b", event);
							firedKeys.push_back("SDLK_b");
						}
					break;

				case SDLK_c:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_c") != "" && mappingContext->GetKeyBoundEvent("SDLK_c").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_c", event);
							firedKeys.push_back("SDLK_c");
						}
					break;

				case SDLK_d:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_d") != "" && mappingContext->GetKeyBoundEvent("SDLK_d").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_d", event);
							firedKeys.push_back("SDLK_d");
						}
					break;

				case SDLK_e:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_e") != "" && mappingContext->GetKeyBoundEvent("SDLK_e").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_e", event);
							firedKeys.push_back("SDLK_e");
						}
					break;

				case SDLK_f:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_f") != "" && mappingContext->GetKeyBoundEvent("SDLK_f").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_f", event);
							firedKeys.push_back("SDLK_f");
						}
					break;

				case SDLK_g:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_g") != "" && mappingContext->GetKeyBoundEvent("SDLK_g").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_g", event);
							firedKeys.push_back("SDLK_g");
						}
					break;

				case SDLK_h:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_h") != "" && mappingContext->GetKeyBoundEvent("SDLK_h").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_h", event);
							firedKeys.push_back("SDLK_h");
						}
					break;

				case SDLK_i:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_i") != "" && mappingContext->GetKeyBoundEvent("SDLK_i").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_i", event);
							firedKeys.push_back("SDLK_i");
						}
					break;

				case SDLK_j:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_j") != "" && mappingContext->GetKeyBoundEvent("SDLK_j").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_j", event);
							firedKeys.push_back("SDLK_j");
						}
					break;

				case SDLK_k:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_k") != "" && mappingContext->GetKeyBoundEvent("SDLK_k").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_k", event);
							firedKeys.push_back("SDLK_k");
						}
					break;

				case SDLK_l:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_l") != "" && mappingContext->GetKeyBoundEvent("SDLK_l").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_l", event);
							firedKeys.push_back("SDLK_l");
						}
					break;

				case SDLK_m:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_m") != "" && mappingContext->GetKeyBoundEvent("SDLK_m").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_m", event);
							firedKeys.push_back("SDLK_m");
						}
					break;

				case SDLK_n:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_n") != "" && mappingContext->GetKeyBoundEvent("SDLK_n").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_n", event);
							firedKeys.push_back("SDLK_n");
						}
					break;

				case SDLK_o:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_o") != "" && mappingContext->GetKeyBoundEvent("SDLK_o").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_o", event);
							firedKeys.push_back("SDLK_o");
						}
					break;

				case SDLK_p:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_p") != "" && mappingContext->GetKeyBoundEvent("SDLK_p").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_p", event);
							firedKeys.push_back("SDLK_p");
						}
					break;

				case SDLK_q:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_q") != "" && mappingContext->GetKeyBoundEvent("SDLK_q").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_q", event);
							firedKeys.push_back("SDLK_q");
						}
					break;

				case SDLK_r:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_r") != "" && mappingContext->GetKeyBoundEvent("SDLK_r").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_r", event);
							firedKeys.push_back("SDLK_r");
						}
					break;

				case SDLK_s:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_s") != "" && mappingContext->GetKeyBoundEvent("SDLK_s").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_s", event);
							firedKeys.push_back("SDLK_s");
						}
					break;

				case SDLK_t:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_t") != "" && mappingContext->GetKeyBoundEvent("SDLK_t").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_t", event);
							firedKeys.push_back("SDLK_t");
						}
					break;

				case SDLK_u:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_u") != "" && mappingContext->GetKeyBoundEvent("SDLK_u").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_u", event);
							firedKeys.push_back("SDLK_u");
						}
					break;

				case SDLK_v:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_v") != "" && mappingContext->GetKeyBoundEvent("SDLK_v").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_v", event);
							firedKeys.push_back("SDLK_v");
						}
					break;

				case SDLK_w:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_w") != "" && mappingContext->GetKeyBoundEvent("SDLK_w").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_w", event);
							firedKeys.push_back("SDLK_w");
						}
					break;

				case SDLK_x:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_x") != "" && mappingContext->GetKeyBoundEvent("SDLK_x").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_x", event);
							firedKeys.push_back("SDLK_x");
						}
					break;

				case SDLK_y:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_y") != "" && mappingContext->GetKeyBoundEvent("SDLK_y").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_y", event);
							firedKeys.push_back("SDLK_y");
						}
					break;

				case SDLK_z:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_z") != "" && mappingContext->GetKeyBoundEvent("SDLK_z").type == 0)
						{
							mappingContext->OnInputEvent("SDLK_z", event);
							firedKeys.push_back("SDLK_z");
						}
					break;

				default:

					break;
				}
			}
			// Keyboard Keys Up
			else if (event.type == SDL_KEYUP)
			{
				// Clear Mapping Context Events of buttons that are released
				switch (event.key.keysym.sym)
				{
				case SDLK_SPACE:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_SPACE") != "")
							mappingContext->ClearInputActionEvent("SDLK_SPACE");
					break;

				case SDLK_UP:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_UP") != "")
							mappingContext->ClearInputActionEvent("SDLK_UP");
					break;

				case SDLK_DOWN:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_DOWN") != "")
							mappingContext->ClearInputActionEvent("SDLK_DOWN");
					break;

				case SDLK_LEFT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_LEFT") != "")
						{
							mappingContext->ClearInputActionEvent("SDLK_LEFT");
							mappingContext->UnFireEvent("SDLK_LEFT");
						}
							
					break;

				case SDLK_RIGHT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_RIGHT") != "")
							mappingContext->ClearInputActionEvent("SDLK_RIGHT");
					break;

				case SDLK_a:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_a") != "")
							mappingContext->ClearInputActionEvent("SDLK_a");
					break;

				case SDLK_b:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_b") != "")
							mappingContext->ClearInputActionEvent("SDLK_b");
					break;

				case SDLK_c:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_c") != "")
							mappingContext->ClearInputActionEvent("SDLK_c");
					break;

				case SDLK_d:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_d") != "")
							mappingContext->ClearInputActionEvent("SDLK_d");
					break;

				case SDLK_e:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_e") != "")
							mappingContext->ClearInputActionEvent("SDLK_e");
					break;

				case SDLK_f:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_f") != "")
							mappingContext->ClearInputActionEvent("SDLK_f");
					break;

				case SDLK_g:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_g") != "")
							mappingContext->ClearInputActionEvent("SDLK_g");
					break;

				case SDLK_h:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_h") != "")
							mappingContext->ClearInputActionEvent("SDLK_h");
					break;

				case SDLK_i:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_i") != "")
							mappingContext->ClearInputActionEvent("SDLK_i");
					break;

				case SDLK_j:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_j") != "")
							mappingContext->ClearInputActionEvent("SDLK_j");
					break;

				case SDLK_k:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_k") != "")
							mappingContext->ClearInputActionEvent("SDLK_k");
					break;

				case SDLK_l:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_l") != "")
							mappingContext->ClearInputActionEvent("SDLK_l");
					break;

				case SDLK_m:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_m") != "")
							mappingContext->ClearInputActionEvent("SDLK_m");
					break;

				case SDLK_n:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_n") != "")
							mappingContext->ClearInputActionEvent("SDLK_n");
					break;

				case SDLK_o:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_o") != "")
							mappingContext->ClearInputActionEvent("SDLK_o");
					break;

				case SDLK_p:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_p") != "")
							mappingContext->ClearInputActionEvent("SDLK_p");
					break;

				case SDLK_q:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_q") != "")
							mappingContext->ClearInputActionEvent("SDLK_q");
					break;

				case SDLK_r:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_r") != "")
							mappingContext->ClearInputActionEvent("SDLK_r");
					break;

				case SDLK_s:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_s") != "")
							mappingContext->ClearInputActionEvent("SDLK_s");
					break;

				case SDLK_t:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_t") != "")
							mappingContext->ClearInputActionEvent("SDLK_t");
					break;

				case SDLK_u:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_u") != "")
							mappingContext->ClearInputActionEvent("SDLK_u");
					break;

				case SDLK_v:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_v") != "")
							mappingContext->ClearInputActionEvent("SDLK_v");
					break;

				case SDLK_w:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_w") != "")
							mappingContext->ClearInputActionEvent("SDLK_w");
					break;

				case SDLK_x:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_x") != "")
							mappingContext->ClearInputActionEvent("SDLK_x");
					break;

				case SDLK_y:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_y") != "")
							mappingContext->ClearInputActionEvent("SDLK_y");
					break;

				case SDLK_z:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("SDLK_z") != "")
							mappingContext->ClearInputActionEvent("SDLK_z");
					break;

				default:

					break;
				}
			}
			// Axis (analog inputs)
			else if (event.type == SDL_JOYAXISMOTION)
			{
				// Axis (analogs)
				//if (event.jaxis.which == 0)
				//{
				switch (event.jaxis.axis)
				{
				case XInputAxis::LeftXAxis:
					// Left of dead zone or right of dead zone
					if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
						event.jaxis.value = 0;
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LeftJoystickX") != "")
							mappingContext->OnInputEvent("XInput_LeftJoystickX", event);
					break;
				case XInputAxis::LeftYAxis:
					// Below dead zone or Above dead zone
					if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
						event.jaxis.value = 0;
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LeftJoystickY") != "")
							mappingContext->OnInputEvent("XInput_LeftJoystickY", event);
					break;
				case XInputAxis::RightXAxis:
					// Left of dead zone or Right of dead zone
					if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
						event.jaxis.value = 0;
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RightJoystick") != "")
							mappingContext->OnInputEvent("XInput_RightJoystick", event);
					break;
				case XInputAxis::RightYAxis:
					// Below dead zone or Above dead zone
					if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
						event.jaxis.value = 0;
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RightJoystick") != "")
							mappingContext->OnInputEvent("XInput_RightJoystick", event);
					break;
				case XInputAxis::LT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LT") != "")
							mappingContext->OnInputEvent("XInput_LT", event);
					break;
				case XInputAxis::RT:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RT") != "")
							mappingContext->OnInputEvent("XInput_RT", event);
					break;
				}
			}
			// Buttons Down
			else if (event.type == SDL_JOYBUTTONDOWN)
			{
				switch (event.jbutton.button)
				{
				case XInputButtons::A:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_A") != "")
						{
							mappingContext->OnInputEvent("XInput_A", event);
							firedKeys.push_back("XInput_A");
						}
							
					break;
				case XInputButtons::B:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_B") != "")
						{
							mappingContext->OnInputEvent("XInput_B", event);
							firedKeys.push_back("XInput_B");
						}
					break;
				case XInputButtons::X:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_X") != "")
						{
							mappingContext->OnInputEvent("XInput_X", event);
							firedKeys.push_back("XInput_X");
						}
					break;
				case XInputButtons::Y:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Y") != "")
						{
							mappingContext->OnInputEvent("XInput_Y", event);
							firedKeys.push_back("XInput_Y");
						}
					break;
				case XInputButtons::LB:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LB") != "")
						{
							mappingContext->OnInputEvent("XInput_LB", event);
							firedKeys.push_back("XInput_LB");
						}
					break;
				case XInputButtons::RB:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RB") != "")
						{
							mappingContext->OnInputEvent("XInput_RB", event);
							firedKeys.push_back("XInput_RB");
						}
					break;
				case XInputButtons::ScreenShot:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_ScreenShot") != "")
						{
							mappingContext->OnInputEvent("XInput_ScreenShot", event);
							firedKeys.push_back("XInput_ScreenShot");
						}
					break;
				case XInputButtons::Start:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Start") != "")
						{
							mappingContext->OnInputEvent("XInput_Start", event);
							firedKeys.push_back("XInput_Start");
						}
					break;
				case XInputButtons::LS:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LS") != "")
						{
							mappingContext->OnInputEvent("XInput_LS", event);
							firedKeys.push_back("XInput_LS");
						}
					break;
				case XInputButtons::RS:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RS") != "")
						{
							mappingContext->OnInputEvent("XInput_RS", event);
							firedKeys.push_back("XInput_RS");
						}
					break;
				case XInputButtons::Home:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Home") != "")
						{
							mappingContext->OnInputEvent("XInput_Home", event);
							firedKeys.push_back("XInput_Home");
						}
					break;
				case XInputButtons::Tray:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Tray") != "")
						{
							mappingContext->OnInputEvent("XInput_Tray", event);
							firedKeys.push_back("XInput_Tray");
						}
					break;
				default:
					break;
				}
			}
			// Buttons Up
			else if (event.type == SDL_JOYBUTTONUP)
			{
				switch (event.jbutton.button)
				{
				case XInputButtons::A:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_A") != "")
							mappingContext->ClearInputActionEvent("XInput_A");
					break;
				case XInputButtons::B:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_B") != "")
							mappingContext->ClearInputActionEvent("XInput_B");
					break;
				case XInputButtons::X:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_X") != "")
							mappingContext->ClearInputActionEvent("XInput_X");
					break;
				case XInputButtons::Y:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Y") != "")
							mappingContext->ClearInputActionEvent("XInput_Y");
					break;
				case XInputButtons::LB:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LB") != "")
							mappingContext->ClearInputActionEvent("XInput_LB");
					break;
				case XInputButtons::RB:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RB") != "")
							mappingContext->ClearInputActionEvent("XInput_RB");
					break;
				case XInputButtons::ScreenShot:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_ScreenShot") != "")
							mappingContext->ClearInputActionEvent("XInput_ScreenShot");
					break;
				case XInputButtons::Start:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)						
						if (mappingContext->GetKeyBinding("XInput_Start") != "")
							mappingContext->ClearInputActionEvent("XInput_Start");
					break;
				case XInputButtons::LS:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_LS") != "")
							mappingContext->ClearInputActionEvent("XInput_LS");
					break;
				case XInputButtons::RS:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_RS") != "")
							mappingContext->ClearInputActionEvent("XInput_RS");
					break;
				case XInputButtons::Home:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Home") != "")
							mappingContext->ClearInputActionEvent("XInput_Home");
					break;
				case XInputButtons::Tray:
					for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
						if (mappingContext->GetKeyBinding("XInput_Tray") != "")
							mappingContext->ClearInputActionEvent("XInput_Tray");
					break;
				default:
					break;
				}
			}
			// Hats
			else if (event.type == SDL_JOYHATMOTION)
			{
				switch (event.jhat.value)
				{
				case XInputHats::Up:
					//LogFloat(event.jhat.type, "Hat Type: ");
					//LogFloat(event.jhat.value, "Hat Value: ");
					//LogFloat(event.jhat.hat, "Hat hat: ");
					break;
				case XInputHats::Down:
					FlatEngine::LogString("Down");
					break;
				case XInputHats::Left:
					FlatEngine::LogString("Left");
					break;
				case XInputHats::Right:
					FlatEngine::LogString("Right");
					break;
				default:
					break;
				}
			}
		}


		// BUTTON COMPONENT EVENTS - GameView
		// 
		// TODO: Check here if the Game viewport is focused before getting the mouse data //
		// Check for mouse over on all of our Game Buttons
		static bool _hasLeftClicked = false;
		static bool _hasRightClicked = false;
		if (FlatEngine::F_UIManager.CheckForMouseOver())
		{
			std::vector<FlatEngine::Button*> hoveredButtons = FlatEngine::F_UIManager.GetHoveredButtons();
			ImGuiIO inputOutput = ImGui::GetIO();
			FlatEngine::Button* topLevelButton = FlatEngine::F_UIManager.GetTopLevelButton();

			// Call the OnMouseOverFunction() in the top level button that is hovered
			GameObject thisObject = FlatEngine::GetObjectById(FlatEngine::F_UIManager.GetTopLevelButton()->GetParentID());

			// If mouse is clicked call the OnLeftClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[0] && !_hasLeftClicked && topLevelButton != nullptr && topLevelButton->LeftClickSet())
			{
				_hasLeftClicked = true;
				topLevelButton->OnLeftClickFunction(&thisObject);
			}
			// Unclick check
			if (!inputOutput.MouseDown[0])
				_hasLeftClicked = false;

			// If mouse is clicked call the OnRightClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[1] && !_hasRightClicked && topLevelButton != nullptr && topLevelButton->RightClickSet())
			{
				_hasRightClicked = true;
				topLevelButton->OnRightClickFunction(&thisObject);
			}
			// Unclick check
			if (!inputOutput.MouseDown[1])
				_hasRightClicked = false;
		}
	}

	void SetFocusedGameObjectID(long ID)
	{
		FocusedGameObjectID = ID;
		if (ID != -1)
		{
			GameObject focusedObject = FlatEngine::GetObjectById(ID);
			Animation* animationComponent = focusedObject.GetAnimationComponent();
			std::string animationPath = "";

			if (animationComponent != nullptr)
				animationPath = animationComponent->GetAnimationPath();

			// If applicable to the current animation, create a copy of the focused GameObject to be used for the animator window.
			if (FlatGui::_showAnimator && FocusedAnimation != nullptr &&
				animationComponent != nullptr && animationPath == FocusedAnimation->animationPath)
			{
				std::vector<GameObject> animatorObjects = std::vector<GameObject>();
				animatorObjects.clear();
				//objectForFocusedAnimation = GameObject(FlatEngine::GetObjectById(ID), animatorObjects, FlatEngine::GetLoadedScene()->GetSceneObjects(), -1);
				FlatEngine::Transform* transform = objectForFocusedAnimation.GetTransformComponent();
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
		std::shared_ptr<Project> newProject = std::make_shared<Project>();
		newProject->SetPath(path);

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
						newProject->SetPath(currentObjectJson["path"]);
					if (currentObjectJson.contains("loadedScenePath"))
						newProject->SetLoadedScenePath(currentObjectJson["loadedScenePath"]);
					if (currentObjectJson.contains("loadedAnimationPath"))
						newProject->SetLoadedPreviewAnimationPath(currentObjectJson["loadedAnimationPath"]);
					if (currentObjectJson.contains("focusedGameObjectID"))
						newProject->SetFocusedGameObjectID(currentObjectJson["focusedGameObjectID"]);

					// Scene Scrolling + Grid Step
					Vector2 sceneViewScroll = Vector2(0, 0);
					if (currentObjectJson.contains("sceneViewScrollingX"))
						sceneViewScroll.x = currentObjectJson["sceneViewScrollingX"];
					if (currentObjectJson.contains("sceneViewScrollingY"))
						sceneViewScroll.y = currentObjectJson["sceneViewScrollingY"];
					newProject->SetSceneViewScrolling(sceneViewScroll);
					Vector2 sceneViewGridStep = Vector2(0, 0);
					if (currentObjectJson.contains("sceneViewGridStepX"))
						sceneViewGridStep.x = currentObjectJson["sceneViewGridStepX"];
					if (currentObjectJson.contains("sceneViewGridStepY"))
						sceneViewGridStep.y = currentObjectJson["sceneViewGridStepY"];
					newProject->SetSceneViewGridStep(sceneViewGridStep);

					// Show/hide windows
					if (currentObjectJson.contains("_showSceneView"))
						FlatGui::_showSceneView = currentObjectJson["_showSceneView"];
					if (currentObjectJson.contains("_showGameView"))
						FlatGui::_showGameView = currentObjectJson["_showGameView"];
					if (currentObjectJson.contains("_showHierarchy"))
						FlatGui::_showHierarchy = currentObjectJson["_showHierarchy"];
					if (currentObjectJson.contains("_showInspector"))
						FlatGui::_showInspector = currentObjectJson["_showInspector"];
					if (currentObjectJson.contains("_showAnimator"))
						FlatGui::_showAnimator = currentObjectJson["_showAnimator"];
					if (currentObjectJson.contains("_showAnimationPreview"))
						FlatGui::_showAnimationPreview = currentObjectJson["_showAnimationPreview"];
					if (currentObjectJson.contains("_showKeyFrameEditor"))
						FlatGui::_showKeyFrameEditor = currentObjectJson["_showKeyFrameEditor"];
					if (currentObjectJson.contains("_showLogger"))
						FlatGui::_showLogger = currentObjectJson["_showLogger"];
					if (currentObjectJson.contains("_showProfiler"))
						FlatGui::_showProfiler = currentObjectJson["_showProfiler"];
					if (currentObjectJson.contains("_showMappingContextEditor"))
						FlatGui::_showMappingContextEditor = currentObjectJson["_showMappingContextEditor"];

					// Settings
					if (currentObjectJson.contains("_clearLogBuffer"))
					{
						FlatGui::_clearBufferEveryFrame = currentObjectJson["_clearLogBuffer"];
						if (FlatGui::_clearBufferEveryFrame)
						{
							FlatEngine::F_Logger.ClearBuffer();
						}
					}
					if (currentObjectJson.contains("_autoSave"))
						newProject->SetAutoSave(currentObjectJson["_autoSave"]);
					if (currentObjectJson.contains("physicsSystem"))
						newProject->SetPhysicsSystem(currentObjectJson["physicsSystem"]);
					if (currentObjectJson.contains("collisionDetection"))
						newProject->SetCollisionDetection(currentObjectJson["collisionDetection"]);
					if (currentObjectJson.contains("resolutionWidth") && currentObjectJson.contains("resolutionHeight"))
						newProject->SetResolution(Vector2(currentObjectJson["resolutionWidth"], currentObjectJson["resolutionHeight"]));
					if (currentObjectJson.contains("_fullscreen"))
						newProject->SetFullscreen(currentObjectJson["_fullscreen"]);
					if (currentObjectJson.contains("_vsyncEnabled"))
						newProject->SetVsyncEnabled(currentObjectJson["_vsyncEnabled"]);
				}
			}
		}

		loadedProject = newProject;

		if (loadedProject->GetLoadedScenePath() != "")
			FlatEngine::LoadScene(loadedProject->GetLoadedScenePath());
		else
			FlatEngine::CreateNewScene();
		if (loadedProject->GetLoadedPreviewAnimationPath() != "")
			SetFocusedAnimation(FlatEngine::LoadAnimationFile(loadedProject->GetLoadedPreviewAnimationPath()));
		Vector2 scrolling = loadedProject->GetSceneViewScrolling();
		FlatGui::sceneViewScrolling = scrolling;
		Vector2 gridStep = loadedProject->GetSceneViewGridStep();
		FlatGui::sceneViewGridStep = gridStep;

		if (loadedProject->GetFocusedGameObjectID() != -1 && FlatEngine::GetObjectById(loadedProject->GetFocusedGameObjectID()) != nullptr)
			SetFocusedGameObjectID(loadedProject->GetFocusedGameObjectID());
	}

	void SaveProject(std::shared_ptr<Project> project, std::string path)
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
			{ "loadedScenePath", project->GetLoadedScenePath()},
			{ "loadedAnimationPath", project->GetLoadedPreviewAnimationPath()},
			{ "focusedGameObjectID", GetFocusedGameObjectID() },
			{ "sceneViewScrollingX", FlatGui::sceneViewScrolling.x },
			{ "sceneViewScrollingY", FlatGui::sceneViewScrolling.y },
			{ "sceneViewGridStepX", FlatGui::sceneViewGridStep.x },
			{ "sceneViewGridStepY", FlatGui::sceneViewGridStep.y },
			{ "_showSceneView", FlatGui::_showSceneView },
			{ "_showGameView", FlatGui::_showGameView },
			{ "_showHierarchy", FlatGui::_showHierarchy },
			{ "_showInspector", FlatGui::_showInspector },
			{ "_showAnimator", FlatGui::_showAnimator },
			{ "_showAnimationPreview", FlatGui::_showAnimationPreview },
			{ "_showKeyFrameEditor", FlatGui::_showKeyFrameEditor },
			{ "_showLogger", FlatGui::_showLogger },
			{ "_showProfiler", FlatGui::_showProfiler },
			{ "_showMappingContextEditor", FlatGui::_showMappingContextEditor },
			{ "_clearLogBuffer", FlatGui::_clearBufferEveryFrame },
			{ "_autoSave", loadedProject->AutoSaveOn() },
			{ "physicsSystem", loadedProject->GetPhysicsSystem() },
			{ "collisionDetection", loadedProject->GetCollisionDetection() },
			{ "resolutionWidth", loadedProject->GetResolution().x },
			{ "resolutionHeight", loadedProject->GetResolution().y },
			{ "_fullscreen", loadedProject->IsFullscreen() },
			{ "_vsyncEnabled", loadedProject->IsVsyncEnabled() },
			});
		projectProperties.push_back(animationName);

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"Project Properties", projectProperties } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	std::shared_ptr<GameObject> GetPlayerObject()
	{
		return playerObject;
	}


	void AddViewports()
	{
		// ImGui Demo Window
		if (_showDemoWindow)
			ImGui::ShowDemoWindow(&_showDemoWindow);

		float startTime = (float)FlatEngine::GetEngineTime();
		MainMenuBar();
		AddProcessData("Render Main Menu Bar", (float)FlatEngine::GetEngineTime() - startTime);

		startTime = (float)FlatEngine::GetEngineTime();
		RenderToolbar();
		AddProcessData("Render Toolbar", (float)FlatEngine::GetEngineTime() - startTime);
		
		if (_showHierarchy)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderHierarchy();
			AddProcessData("Render Hierarchy", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showInspector)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderInspector();
			AddProcessData("Render Inspector", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showGameView)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			Game_RenderView();
			AddProcessData("Render Game View", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showSceneView)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			Scene_RenderView();
			AddProcessData("Render Scene View", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showAnimator)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderAnimator();
			AddProcessData("Render Animator", (float)FlatEngine::GetEngineTime() - startTime);
		}
		
		if (_showAnimationPreview)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderAnimationPreview();
			AddProcessData("Render Animation Preview", (float)FlatEngine::GetEngineTime() - startTime);
		}
		
		if (_showKeyFrameEditor)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderKeyFrameEditor();
			AddProcessData("Render Key Frame Editor", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showLogger)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderLog();
			AddProcessData("Render Log", (float)FlatEngine::GetEngineTime() - startTime);
		}
	
		if (_showProfiler)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderProfiler();
			AddProcessData("Render Profiler", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showMappingContextEditor)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderMappingContextEditor();
			AddProcessData("Render Mapping Context Editor", (float)FlatEngine::GetEngineTime() - startTime);
		}

		if (_showSettings)
		{
			startTime = (float)FlatEngine::GetEngineTime();
			RenderSettings();
			AddProcessData("Render Settings", (float)FlatEngine::GetEngineTime() - startTime);
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

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

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
		Transform* transform = self.GetTransformComponent();
		Sprite* sprite = self.GetSpriteComponent();
		Camera* camera = self.GetCameraComponent();
		Button* button = self.GetButtonComponent();
		Canvas* canvas = self.GetCanvasComponent();
		Text* text = self.GetTextComponent();
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

				Vector2 positionOnScreen = Vector2(sceneViewCenter.x - canvas_p0.x + (position.x * step) - ((pivotOffset.x * spriteScaleMultiplier * step) * scale.x * spriteScale.x), sceneViewCenter.y - canvas_p0.y - (position.y * step - 20) - ((pivotOffset.y * spriteScaleMultiplier * step) * scale.y * spriteScale.y));
				ImGui::SetCursorPos(positionOnScreen);
				//// This will catch our interactions  - 4096 for overlap or keyword if it works
				ImGui::SetNextItemAllowOverlap();
				ImGui::InvisibleButton(invisibleButtonID.c_str(), Vector2(spriteTextureWidth * spriteScaleMultiplier * step * scale.x * spriteScale.x, spriteTextureHeight * spriteScaleMultiplier * step * scale.y * spriteScale.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
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
					sceneViewScrolling.x += inputOutput.MouseDelta.x;
					sceneViewScrolling.y += inputOutput.MouseDelta.y;						
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
							sceneViewScrolling.x -= trunc(signedMousePosX * weight);
							sceneViewScrolling.y -= trunc(signedMousePosY * weight);
						}
						sceneViewGridStep.x += finalZoomSpeed;
						sceneViewGridStep.y += finalZoomSpeed;
					}
					else if (scrollInput < 0 && sceneViewGridStep.x > 2 && sceneViewGridStep.y > 2)
					{
						if (_weightedScroll)
						{
							sceneViewScrolling.x += trunc(signedMousePosX * weight);
							sceneViewScrolling.y += trunc(signedMousePosY * weight);
						}
						sceneViewGridStep.x -= finalZoomSpeed;
						sceneViewGridStep.y -= finalZoomSpeed;
					}
				}
				//////////////////

				// Change the draw channel for the scene object
				if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
					drawSplitter->SetCurrentChannel(draw_list, renderOrder);
				else
					drawSplitter->SetCurrentChannel(draw_list, 0);

				// Draw the texture
				AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, pivotOffset, Vector2(transformScale.x * spriteScale.x, transformScale.y * spriteScale.y), _spriteScalesWithZoom, step, draw_list, rotation, ImGui::GetColorU32(tintColor));
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
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(textTexture.GetTexture(), position, sceneViewCenter, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, sceneViewGridStep.x, draw_list, rotation);
				}
			}

			// Renders the camera
			if (camera != nullptr)
			{
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = scrolling.x + (position.x * sceneViewGridStep.x) - (cameraWidth * sceneViewGridStep.x / 2 * scale.x);
				float cameraTopEdge = scrolling.y + (-position.y * sceneViewGridStep.y) - (cameraHeight * sceneViewGridStep.y / 2 * scale.y);
				float cameraRightEdge = scrolling.x + (position.x * sceneViewGridStep.x) + (cameraWidth * sceneViewGridStep.x / 2 * scale.x);
				float cameraBottomEdge = scrolling.y + (-position.y * sceneViewGridStep.y) + (cameraHeight * sceneViewGridStep.y / 2 * scale.y);

				Vector2 topLeftCorner = Vector2(cameraLeftEdge, cameraTopEdge);
				Vector2 bottomRightCorner = Vector2(cameraRightEdge, cameraBottomEdge);
				Vector2 topRightCorner = Vector2(cameraRightEdge, cameraTopEdge);
				Vector2 bottomLeftCorner = Vector2(cameraLeftEdge, cameraBottomEdge);

				float cameraTextureWidth = (float)cameraIcon.GetWidth() / 4;
				float cameraTextureHeight = (float)cameraIcon.GetHeight() / 4;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, cameraBoxColor, 2.0f, draw_list);

				// Draw actual camera icon
				AddImageToDrawList(cameraIcon.GetTexture(), position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, step, draw_list, 0, IM_COL32(255, 255, 255, iconTransparency));
			}

			// Renders Canvas Component
			if (canvas != nullptr)
			{
				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float renderXStart = sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2)) * sceneViewGridStep.x);										
				float renderYStart = sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2)) * sceneViewGridStep.x);
				Vector2 renderStart = Vector2(renderXStart, renderYStart);
				Vector2 renderEnd = Vector2(renderXStart + ((activeWidth * transformScale.x) * sceneViewGridStep.x), renderYStart + ((activeHeight * transformScale.y) * sceneViewGridStep.x));

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				FlatEngine::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, canvasBorderColor, 3.0f, draw_list);
			}

			// Renders Button Component
			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				float activeLeft = sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * sceneViewGridStep.x);
				float activeRight = sceneViewCenter.x + ((position.x + (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * sceneViewGridStep.x);
				float activeTop = sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * sceneViewGridStep.y);
				float activeBottom = sceneViewCenter.y - ((position.y - (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * sceneViewGridStep.y);

				Vector2 center = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };
				Vector2 topRight = { activeRight, activeTop };
				Vector2 bottomLeft = { activeLeft, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (rotation != 0)
				{
					float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
					float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

					topLeft = ImRotate(Vector2(-activeWidth * sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					topRight = ImRotate(Vector2(+activeWidth * sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					bottomRight = ImRotate(Vector2(+activeWidth * sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);
					bottomLeft = ImRotate(Vector2(-activeWidth * sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * sceneViewGridStep.x / 2 * transformScale.y), cos_a, sin_a);

					Vector2 pos[4] =
					{
						Vector2(center.x + topLeft.x, center.y + topLeft.y),
						Vector2(center.x + topRight.x, center.y + topRight.y),
						Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
						Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
					};

					if (_isActive)
					{
						FlatEngine::DrawLine(pos[0], pos[1], buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[1], pos[2], buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[2], pos[3], buttonComponentActiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(pos[3], pos[0], buttonComponentActiveColor, 2.0f, draw_list);
					}
					else
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentInctiveColor, 1.0f, draw_list);
				}
				else
				{
					if (_isActive)
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentActiveColor, 1.0f, draw_list);
					else
						FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentInctiveColor, 1.0f, draw_list);
				}
			}

			// Renders BoxCollider Component
			if (boxColliders.size() > 0)
			{
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

					boxCollider->UpdateActiveEdges(loadedProject->GetCollisionDetection(), sceneViewGridStep.x, sceneViewCenter);

					Vector2 corners[4] = {
						boxCollider->GetCorners()[0],
						boxCollider->GetCorners()[1],
						boxCollider->GetCorners()[2],
						boxCollider->GetCorners()[3],
					};

					drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

					if (loadedProject->GetCollisionDetection() == "Shared Axis")
					{
						if (_isActive && !_isColliding)
							FlatEngine::DrawRectangleFromLines(corners, colliderActiveColor, 1.0f, draw_list);
						else if (!_isActive)
							FlatEngine::DrawRectangleFromLines(corners, colliderInactiveColor, 1.0f, draw_list);
						else if (_isColliding)
							FlatEngine::DrawRectangleFromLines(corners, colliderCollidingColor, 1.0f, draw_list);
					}
					else if (loadedProject->GetCollisionDetection() == "Separating Axis")
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
						FlatEngine::DrawLine(center, normals[0], colliderInactiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(center, normals[1], colliderInactiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(center, normals[2], colliderInactiveColor, 2.0f, draw_list);
						FlatEngine::DrawLine(center, normals[3], colliderInactiveColor, 2.0f, draw_list);

						if (_isActive && !_isColliding)
							FlatEngine::DrawRectangleFromLines(corners, colliderActiveColor, 1.0f, draw_list);
						else if (!_isActive)
							FlatEngine::DrawRectangleFromLines(corners, colliderInactiveColor, 1.0f, draw_list);
						else if (_isColliding)
							FlatEngine::DrawRectangleFromLines(corners, colliderCollidingColor, 1.0f, draw_list);
					}

					// Draw activeRadius circle
					if (_showActiveRadius)
						FlatEngine::DrawCircle(center, activeRadius, colliderActiveColor, draw_list);
				}
			}

			// Renders CircleCollider Component
			if (circleColliders.size() > 0)
			{
				for (CircleCollider* circleCollider : circleColliders)
				{
					Vector2 activeOffset = circleCollider->GetActiveOffset();
					int activeLayer = circleCollider->GetActiveLayer();
					bool _isActive = circleCollider->IsActive();
					bool _isColliding = circleCollider->IsColliding();
					float activeRadius = circleCollider->GetActiveRadiusGrid() * sceneViewGridStep.x;
					circleCollider->SetActiveRadiusScreen(activeRadius);
					bool _showActiveRadius = circleCollider->GetShowActiveRadius();
					Vector2 center = circleCollider->GetCenterCoord();

					drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

					circleCollider->UpdateActiveEdges(loadedProject->GetCollisionDetection(), sceneViewGridStep.x, sceneViewCenter);

					if (_isActive && !_isColliding)
						FlatEngine::DrawCircle(center, activeRadius, colliderActiveColor, draw_list);
					else if (!_isActive)
						FlatEngine::DrawCircle(center, activeRadius, colliderInactiveColor, draw_list);
					else if (_isColliding)
						FlatEngine::DrawCircle(center, activeRadius, colliderCollidingColor, draw_list);
				}
			}

			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most -- If this obect is focused
			if (focusedObjectID != -1 && focusedObjectID == self.GetID())
			{
				GameObject focusedObject = FlatEngine::GetObjectById(focusedObjectID);
				SDL_Texture* arrowToRender = transformArrow.GetTexture();
				// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)transformArrow.GetWidth() * 3;
				float arrowHeight = (float)transformArrow.GetHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 3, arrowHeight - 3 };
				bool _scalesWithZoom = false;
				float transformMoveModifier = 0.02f;
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 positionOnScreen = Vector2(sceneViewCenter.x + (position.x * step), sceneViewCenter.y - (position.y * step));

				// Invisible button for Transform Arrow Move X and Y
				Vector2 moveAllStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
				RenderInvisibleButton("TransformBaseArrowButton", moveAllStartPos, Vector2(28, 28), false);
				const bool _baseHovered = ImGui::IsItemHovered();
				const bool _baseActive = ImGui::IsItemActive();
				const bool _baseClicked = ImGui::IsItemClicked();

				if (_baseHovered || _baseActive)
				{
					arrowToRender = transformArrowAllWhite.GetTexture();
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for X arrow
				Vector2 moveXStartPos = Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
				RenderInvisibleButton("TransformBaseArrowXButton", moveXStartPos, Vector2(63, 35), false);
				const bool _xHovered = ImGui::IsItemHovered();
				const bool _xActive = ImGui::IsItemActive();
				const bool _xClicked = ImGui::IsItemClicked();
				if (_xHovered || _xActive)
				{
					arrowToRender = transformArrowXWhite.GetTexture();
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for Y arrow
				Vector2 moveYStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
				RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, Vector2(35, 63), false);
				const bool _yHovered = ImGui::IsItemHovered();
				const bool _yActive = ImGui::IsItemActive();
				const bool _yClicked = ImGui::IsItemClicked();
				if (_yHovered || _yActive)
				{
					arrowToRender = transformArrowYWhite.GetTexture();
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
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 3);
				AddImageToDrawList(arrowToRender, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, step, draw_list);
			}
		}
	}


	Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation, ImU32 addColor)
	{
		// Changing the scale here because sprites are rendering too large and I want them to start off smaller and also keep the default scale value to 1.0f
		Vector2 newScale = Vector2(scale.x * spriteScaleMultiplier, scale.y * spriteScaleMultiplier);

		float scalingXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - (offsetPx.x * newScale.x * zoomMultiplier);
		float scalingYStart = relativeCenterPoint.y - (positionInGrid.y * zoomMultiplier) - (offsetPx.y * newScale.y * zoomMultiplier);
		float scalingXEnd = scalingXStart + (textureWidthPx * newScale.x * zoomMultiplier);
		float scalingYEnd = scalingYStart + (textureHeightPx * newScale.y * zoomMultiplier);

		float unscaledXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - offsetPx.x * scale.x;
		float unscaledYStart = relativeCenterPoint.y + (-positionInGrid.y * zoomMultiplier) - offsetPx.y * scale.y;

		Vector2 renderStart;
		Vector2 renderEnd;
		Vector2 UvStart = { 0, 0 };
		Vector2 UvEnd = { 1, 1 };

		if (_scalesWithZoom)
		{
			renderStart = Vector2(scalingXStart, scalingYStart);
			renderEnd = Vector2(scalingXEnd, scalingYEnd);
			//DrawRectangle(renderStart, renderEnd, Vector2(0,0), Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), whiteColor, 2, draw_list);
		}
		else
		{
			renderStart = Vector2(unscaledXStart, unscaledYStart);
			renderEnd = Vector2(renderStart.x + textureWidthPx * scale.x, renderStart.y + textureHeightPx * scale.y);
		}

		if (rotation != 0)
		{
			float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
			float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

			Vector2 topLeft = ImRotate(Vector2(-(renderEnd.x - renderStart.x) / 2, -(renderEnd.y - renderStart.y) / 2), cos_a, sin_a);
			Vector2 topRight = ImRotate(Vector2(+(renderEnd.x - renderStart.x) / 2, -(renderEnd.y - renderStart.y) / 2), cos_a, sin_a);
			Vector2 bottomRight = ImRotate(Vector2(+(renderEnd.x - renderStart.x) / 2, (renderEnd.y - renderStart.y) / 2), cos_a, sin_a);
			Vector2 bottomLeft = ImRotate(Vector2(-(renderEnd.x - renderStart.x) / 2, +(renderEnd.y - renderStart.y) / 2), cos_a, sin_a);

			Vector2 center = Vector2(renderStart.x + ((renderEnd.x - renderStart.x) / 2), renderStart.y + ((renderEnd.y - renderStart.y) / 2));
			Vector2 pos[4] =
			{
				Vector2(center.x + topLeft.x, center.y + topLeft.y),
				Vector2(center.x + topRight.x, center.y + topRight.y),
				Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
				Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
			};
			Vector2 uvs[4] =
			{
				Vector2(0.0f, 0.0f),
				Vector2(1.0f, 0.0f),
				Vector2(1.0f, 1.0f),
				Vector2(0.0f, 1.0f)
			};

			// Render sprite to viewport
			draw_list->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
		}
		else
		{
			// Render sprite to viewport
			draw_list->AddImage((void*)texture, renderStart, renderEnd, UvStart, UvEnd, addColor);
		}

		return renderStart;
	}

	ImU32 Vector4ToImU32(Vector4 color)
	{
		float redValue = color.x;
		float greenValue = color.y;
		float blueValue = color.z;
		float alphaValue = color.w;
		return (((ImU32)(alphaValue) << 24) | ((ImU32)(blueValue) << 16) | ((ImU32)(greenValue) << 8) | ((ImU32)(redValue)));
	}

	std::vector<std::shared_ptr<GameObject>> RayCast(Vector2 origin, Vector2 direction, float distance)
	{
		Vector2 correctedOrigin = Vector2(FlatGui::sceneViewCenter.x + origin.x * FlatGui::sceneViewGridStep.x, FlatGui::sceneViewCenter.y - origin.y * FlatGui::sceneViewGridStep.y);
		float hypotenuse = sqrt(direction.x * direction.x + direction.y * direction.y);
		float divisor = hypotenuse / distance;
		Vector2 endPoint = Vector2(correctedOrigin.x + direction.x / divisor * FlatGui::sceneViewGridStep.x, correctedOrigin.y - direction.y / divisor * FlatGui::sceneViewGridStep.y);

		//ImGui::Begin("Scene View");
		//ImDrawList* drawList = ImGui::GetWindowDrawList();
		//DrawLine(correctedOrigin, endPoint, FlatGui::whiteColor, 3, drawList);
		//ImGui::End();
		return std::vector<std::shared_ptr<GameObject>>();
	}

	std::vector<std::shared_ptr<GameObject>> SphereCast(Vector2 origin, Vector2 direction, float distance)
	{
		return std::vector<std::shared_ptr<GameObject>>();
	}
}