#include "FlatEngine.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Logger.h"
#include "ScriptComponent.h"
#include "Transform.h"
#include "Sprite.h"
#include <cmath>
#include <Windows.h>
#include <string>
#include <shobjidl.h> 
#include <Commdlg.h>
#include "implot.h"
#include "implot_internal.h"
#include "Process.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ImSequencer.h"
#include "Text.h"
#include "Scene.h"
#include "Camera.h"

#include "MappingContext.h"


/*
######################################
######							######
######   ImGui implimentation   ######
######                          ######
######################################
*/


namespace FlatEngine { namespace FlatGui {

	// For window styles
	float childPadding = 8;
	ImVec4 outerWindowColor = ImVec4(float(0.15), float(0.15), float(0.15), float(1));
	ImVec4 innerWindowColor = ImVec4(float(0.19), float(0.19), float(0.19), float(1));
	ImVec4 singleItemColor = ImVec4(float(0.15), float(0.15), float(0.15), float(1));
	ImVec4 singleItemDark = ImVec4(float(0.09), float(0.09), float(0.13), float(1));

	// For rendering sprites
	int maxSpriteLayers = 55;
	float spriteScaleMultiplier = 0.2f;

	// Frame Counter
	int framesDrawn = 0;

	// Settings
	int iconTransparency = 100;

	// Window Visibility
	bool _showSceneView = true;
	bool _showGameView = true;
	bool _showHierarchy = true;
	bool _showInspector = true;
	bool _showAnimator = true;
	bool _showAnimationPreview = true;
	bool _showKeyFrameEditor = true;
	bool _showLogger = true;
	bool _showProfiler = true;

	ImVec2 worldCenterPoint = ImVec2(0, 0);


	using ComponentTypes = Component::ComponentTypes;


	void SetupImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowPadding = { 5.0f, 5.0f };
		style.DockingSeparatorSize = 1;
		style.SeparatorTextAlign = ImVec2(0.5f, 0.0f);
		style.SeparatorTextBorderSize = 1;

		ImGui_ImplSDL2_InitForSDLRenderer(Window::window, Window::renderer);
		ImGui_ImplSDLRenderer2_Init(Window::renderer);


		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Load in what is currently in SavedScenes.json
			FlatEngine::LoadScene("MainMenu.json");

			// Remove the reference to the imgui.ini file for layout since we only need that in Engine mode and
			// we don't want to have to include it in the final release build anyway.
			io.IniFilename = NULL;

			// Set fullscreen here for now
			Window::SetFullscreen(true);
		}
		else
			CreateNewScene();
	}

	void RunOnceAfterInitialization()
	{
		static bool _initialized = false;
		static bool _hasRunOnce = false;

		if (_initialized && !_hasRunOnce)
		{
			// Initialize Mapping Contexts
			InitializeMappingContexts();

			// Open Project by default
			OpenProject("C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\WindowsApplication\\projects\\Sandbox.json");
			
			_hasRunOnce = true;
		}

		_initialized = true;
	}

	void Render(bool& quit)
	{
		HandleEvents(quit);

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Create dockable background space for all viewports
		ImGui::DockSpaceOverViewport();

		//Add viewport(s)
		// 
		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Just Add GameView
			Game_RenderView();
		}
		// Else add FlatEngine viewports
		else
			AddViewports();

		// Rendering
		ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		SDL_RenderSetScale(Window::renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(Window::renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(Window::renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(Window::renderer);

		// For things we only want to execute once after complete initialization
		RunOnceAfterInitialization();
	}

	void HandleEvents(bool &quit)
	{
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
			else if (event.type == SDL_JOYAXISMOTION)
			{
				// Joystick Direction
				static int leftXDir = 0;
				static int leftYDir = 0;
				static int rightXDir = 0;
				static int rightYDir = 0;

				// Axis (analogs)
				if (event.jaxis.which == 0)
				{
					switch (event.jaxis.axis)
					{
					case XboxAxis::LeftXAxis:
						// Left of dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							leftXDir = event.jaxis.value;
						// Right of dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							leftXDir = event.jaxis.value;
						else
							leftXDir = 0;
						break;
					case XboxAxis::LeftYAxis:
						// Below dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							leftYDir = event.jaxis.value * -1;
						// Above dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							leftYDir = event.jaxis.value * -1;
						else
							leftYDir = 0;
						break;
					case XboxAxis::RightXAxis:
						// Left of dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							rightXDir = event.jaxis.value;
						// Right of dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							rightXDir = event.jaxis.value;
						else
							rightXDir = 0;
						break;
					case XboxAxis::RightYAxis:
						// Below dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							rightYDir = event.jaxis.value * -1;
						// Above dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							rightYDir = event.jaxis.value * -1;
						else
							rightYDir = 0;
						break;
					case XboxAxis::LT:
						LogFloat(event.jaxis.value, "Left Trigger: ");
						break;
					case XboxAxis::RT:
						LogFloat(event.jaxis.value, "Rght Trigger: ");
						break;
					}

					//Calculate angle
					float leftJoystickAngle = atan2((float)leftYDir, (float)leftYDir) * (180 / M_PI);

					//Correct angle
					if (leftXDir == 0 && leftYDir == 0)
						leftJoystickAngle = 0;

					//LogFloat(leftJoystickAngle, "Left Joystic Angle: ");
					LogFloat(leftXDir, "Left X: ");
					LogFloat(leftYDir, "Left Y: ");
					LogFloat(rightXDir, "Right X: ");
					LogFloat(rightYDir, "Right Y: ");
				}
			}
			// Buttons
			else if (event.type == SDL_JOYBUTTONDOWN)
			{
				switch (event.jbutton.button)
				{
				case XboxButtons::A:
					LogString("A");
					break;
				case XboxButtons::B:
					LogString("B");
					break;
				case XboxButtons::X:
					LogString("X");
					break;
				case XboxButtons::Y:
					LogString("Y");
					break;
				case XboxButtons::LB:
					LogString("LB");
					break;
				case XboxButtons::RB:
					LogString("RB");
					break;
				case XboxButtons::ScreenShot:
					LogString("ScreenShot");
					break;
				case XboxButtons::Start:
					LogString("Start");
					break;
				case XboxButtons::LS:
					LogString("LS");
					break;
				case XboxButtons::RS:
					LogString("RS");
					break;
				case XboxButtons::Home:
					LogString("Home");
					break;
				case XboxButtons::Tray:
					LogString("Tray");
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
				case XboxHats::Up:
					LogString("Up");
					break;
				case XboxHats::Down:
					LogString("Down");
					break;
				case XboxHats::Left:
					LogString("Left");
					break;
				case XboxHats::Right:
					LogString("Right");
					break;
				default:
					break;
				}
			}
		}
	}

	std::string OpenSaveFileExplorer()
	{
		std::string sSelectedFile;
		std::string sFilePath;
		HRESULT hr = 0;
		wchar_t* pSaveFileName = NULL;
		IShellItem* pShellItem = NULL;
		wchar_t* ppszName = NULL;
		
		//  CREATE FILE OBJECT INSTANCE
		HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr))
		return "";

		// CREATE FileSaveDialog OBJECT
		 IFileSaveDialog* f_FileSystem = NULL;
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, (void**)(&f_FileSystem));
		if (FAILED(f_SysHr)) {
			CoUninitialize();
			return "";
		}

		//  SHOW OPEN FILE DIALOG WINDOW
		f_SysHr = f_FileSystem->Show(NULL);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  STORE AND CONVERT THE FILE NAME
		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) {
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  FORMAT AND STORE THE FILE PATH
		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		sFilePath = c;

		//  FORMAT STRING FOR EXECUTABLE NAME
		const size_t slash = sFilePath.find_last_of("/\\");
		sSelectedFile = sFilePath.substr(slash + 1);

		//  SUCCESS, CLEAN UP
		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return sFilePath;
	}

	std::string OpenLoadFileExplorer()
	{
		std::string sSelectedFile;
		std::string sFilePath;
		HRESULT hr = 0;
		wchar_t* pSaveFileName = NULL;
		IShellItem* pShellItem = NULL;
		wchar_t* ppszName = NULL;

		//  CREATE FILE OBJECT INSTANCE
		HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr))
		return "";

		// CREATE FileOpenDialog OBJECT
		IFileOpenDialog* f_FileSystem;
		f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
		if (FAILED(f_SysHr)) {
			CoUninitialize();
			return "";
		}

		//  SHOW OPEN FILE DIALOG WINDOW
		f_SysHr = f_FileSystem->Show(NULL);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  STORE AND CONVERT THE FILE NAME
		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) {
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  FORMAT AND STORE THE FILE PATH
		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		sFilePath = c;

		//  FORMAT STRING FOR EXECUTABLE NAME
		const size_t slash = sFilePath.find_last_of("/\\");
		sSelectedFile = sFilePath.substr(slash + 1);

		//  SUCCESS, CLEAN UP
		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return sFilePath;
	}

	void AddViewports()
	{
		MainMenuBar();
		RenderToolbar();
		if (_showHierarchy)
			RenderHierarchy();
		if (_showInspector)
			RenderInspector();
		if (_showGameView)
			Game_RenderView();
		if (_showSceneView)
			Scene_RenderView();
		if (_showAnimator)
			RenderAnimator();
		if (_showAnimationPreview)
			RenderAnimationPreview();
		if (_showKeyFrameEditor)
			RenderKeyFrameEditor();
		if (_showLogger)
			RenderLog();
		if (_showProfiler)
			RenderProfiler();
	}

	void RenderGridView(ImVec2& centerPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, ImVec2 step, ImVec2 centerOffset)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		ImVec2 adjustedScrolling = ImVec2(scrolling.x + centerOffset.x, scrolling.y + centerOffset.y);
		

		// Our grid step determines the largest gap between each grid point so our centerpoints must fall on
		// one of those gridstep locations. We get the total grid steps that will render given the current viewport
		// size and divide that by two to get the closest spot to the center of the viewport. It's okay that this
		// is not exactly center at all, the viewport width will never be the perfect size, we just need a starting
		// point and for that point. We need to update this value every pass of the scene view because the gridStep
		// value will change over time and we need to keep these in sync.          
		// 
		//                   V
		// |  |  |  |  |  |  |  |  |  |  |  |  |

		// X = 0 starts the drawing at the left most edge of the entire app window.

		// Draw horizontal grid lines
		for (float x = trunc(fmodf(adjustedScrolling.x + canvas_p0.x, step.y)); x < canvas_p0.x + canvas_sz.x; x += step.y)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
		}
		// Draw vertical grid lines
		for (float y = trunc(fmodf(adjustedScrolling.y + canvas_p0.y, step.y)); y < canvas_p0.y + canvas_sz.y; y += step.y)
		{
			FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
		}

		// Draw our x and y axis blue and green lines
		//
		float divX = trunc(adjustedScrolling.x / step.x);
		float modX = fmodf(adjustedScrolling.x, step.x);
		float offsetX = (step.x * divX) + modX;
		float divY = trunc(adjustedScrolling.y / step.y);
		float modY = fmodf(adjustedScrolling.y, step.y);
		float offsetY = (step.y * divY) + modY;

		// Blue, green and pink colors for axis and center
		ImU32 xColor = IM_COL32(1, 210, 35, 255);
		ImU32 yColor = IM_COL32(50, 50, 255, 255);
		ImU32 centerColor = IM_COL32(255, 1, 247, 255);

		centerPoint = ImVec2(offsetX + canvas_p0.x, offsetY + canvas_p0.y);
		float drawYAxisAt = centerPoint.x;
		float drawXAxisAt = centerPoint.y;

		// x axis bounds check + color change (lighten) if out of bounds
		if (centerPoint.x > canvas_p1.x - 1)
		{
			drawYAxisAt = canvas_p1.x - 1;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		else if (centerPoint.x < canvas_p0.x)
		{
			drawYAxisAt = canvas_p0.x;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		// y axis bounds check + color change (lighten) if out of bounds
		if (centerPoint.y > canvas_p1.y - 1)
		{
			drawXAxisAt = canvas_p1.y - 1;
			yColor = IM_COL32(20, 20, 200, 150);
		}
		else if (centerPoint.y < canvas_p0.y)
		{
			drawXAxisAt = canvas_p0.y;
			yColor = IM_COL32(20, 20, 200, 150);
		}


		// Draw the axis and center point
		FlatEngine::DrawLine(ImVec2(drawYAxisAt, canvas_p0.y), ImVec2(drawYAxisAt, canvas_p1.y), xColor, 1.0f, draw_list);
		FlatEngine::DrawLine(ImVec2(canvas_p0.x, drawXAxisAt), ImVec2(canvas_p1.x, drawXAxisAt), yColor, 1.0f, draw_list);
		FlatEngine::DrawPoint(ImVec2(centerPoint.x, centerPoint.y), centerColor, draw_list);
	}

	void RenderViewObjects(std::vector<std::shared_ptr<GameObject>> objects, ImVec2 centerPoint, ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

		// Loop through scene objects
		for (std::shared_ptr<GameObject> object : objects)
		{
			// If this Scene Object doesn't have a parent, render it and all of its children
			if (object->GetParentID() == -1 && object->IsActive())
			{
				// Start off with a 0,0 parentOffset because this is the top level object to be rendered.
				Vector2 parentOffset(0, 0);
				Vector2 parentScale(1, 1);

				// Render self and children recursively
				RenderSelfThenChildren(object, parentOffset, parentScale, centerPoint, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}

		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}

	void RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter)
	{
		std::shared_ptr<Transform> transform = self->GetTransformComponent();
		std::shared_ptr<Sprite> sprite = self->GetSpriteComponent();
		std::shared_ptr<Camera> camera = self->GetCameraComponent();
		std::shared_ptr<Button> button = self->GetButtonComponent();
		std::shared_ptr<Canvas> canvas = self->GetCanvasComponent();
		std::shared_ptr<Text> text = self->GetTextComponent();

		// Check if each object has a Transform component
		if (transform != nullptr)
		{
			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			Vector2 position = Vector2(transform->GetPosition().x + parentOffset.x, transform->GetPosition().y + parentOffset.y);
			Vector2 transformScale = Vector2(transform->GetScale().x * parentScale.x, transform->GetScale().y * parentScale.y);

			// If it has a sprite component, render that sprite texture at the objects transform position with offsets
			if (sprite != nullptr)
			{
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float spriteTextureWidth = (float)sprite->GetTextureWidth();
				float spriteTextureHeight = (float)sprite->GetTextureHeight();
				Vector2 spriteOffset = sprite->GetOffset();
				bool _spriteScalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();

				// If there is a valid Texture loaded into the Sprite Component
				if (spriteTexture != nullptr)
				{
					// Change the draw channel for the scene object
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
				}
			}

			// If it has a text component, render that text texture at the objects transform position
			if (text != nullptr)
			{
				std::shared_ptr<Texture> textTexture = text->GetTexture();
				text->LoadText();
				SDL_Texture* texture = textTexture->getTexture();
				float textWidth = (float)textTexture->getWidth();
				float textHeight = (float)textTexture->getHeight();
				int renderOrder = text->GetRenderOrder();
				Vector2 offset = text->GetOffset();
				bool _spriteScalesWithZoom = true;


				// If there is a valid Texture loaded into the Sprite Component
				if (textTexture != nullptr)
				{
					// Change the draw channel for the scene object
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(textTexture->getTexture(), position, scrolling, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
				}
			}

			// Renders the camera
			if (camera != nullptr)
			{
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = WorldToViewport(scrolling.x, position.x - cameraWidth / 2 * transformScale.x, gridStep);
				float cameraRightEdge = WorldToViewport(scrolling.x, position.x + cameraWidth / 2 * transformScale.x, gridStep);
				float cameraTopEdge = WorldToViewport(scrolling.y, -(position.y + cameraHeight / 2 * transformScale.y), gridStep);
				float cameraBottomEdge = WorldToViewport(scrolling.y, -(position.y - cameraHeight / 2 * transformScale.y), gridStep);

				ImVec2 topLeftCorner = ImVec2(cameraLeftEdge, cameraTopEdge);
				ImVec2 bottomRightCorner = ImVec2(cameraRightEdge, cameraBottomEdge);
				ImVec2 topRightCorner = ImVec2(cameraRightEdge, cameraTopEdge);
				ImVec2 bottomLeftCorner = ImVec2(cameraLeftEdge, cameraBottomEdge);

				cameraTexture->loadFromFile("assets/images/camera.png");
				SDL_Texture* texture = cameraTexture->getTexture();
				float cameraTextureWidth = (float)cameraTexture->getWidth() * 3;
				float cameraTextureHeight = (float)cameraTexture->getHeight() * 3;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
				FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
				FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);

				// Draw actual camera icon
				AddImageToDrawList(texture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, iconTransparency));
			}

			// Renders Canvas Component
			if (canvas != nullptr)
			{
				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float canvasLeft = WorldToViewport(scrolling.x, position.x - activeWidth / 2, gridStep, false);
				float canvasRight = WorldToViewport(scrolling.x, position.x + activeWidth / 2, gridStep, false);
				float canvasTop = WorldToViewport(scrolling.y, position.y + activeHeight / 2, gridStep, true);
				float canvasBottom = WorldToViewport(scrolling.y, position.y - activeHeight / 2, gridStep, true);

				Vector2 canvasTopLeft = { canvasLeft, canvasTop };
				Vector2 canvasBottomRight = { canvasRight, canvasBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				FlatEngine::DrawRectangle(canvasTopLeft, canvasBottomRight, canvas_p0, canvas_sz, FlatEngine::CanvasBorder, 3.0f, draw_list);
			}


			// Renders Button Component
			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				float activeLeft = WorldToViewport(scrolling.x, position.x + activeOffset.x - (activeWidth / 2 * transformScale.x), gridStep, false);
				float activeRight = WorldToViewport(scrolling.x, position.x + activeOffset.x + (activeWidth / 2 * transformScale.x), gridStep, false);
				float activeTop = WorldToViewport(scrolling.y, position.y + activeOffset.y + (activeHeight / 2 * transformScale.y), gridStep, true);
				float activeBottom = WorldToViewport(scrolling.y, position.y + activeOffset.y - (activeHeight / 2 * transformScale.y), gridStep, true);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (_isActive)
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::ActiveButtonColor, 3.0f, draw_list);
				else
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::InactiveButtonColor, 3.0f, draw_list);
			}


			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most -- If this obect is focused
			if (focusedObjectID != -1 && focusedObjectID == self->GetID())
			{
				// Get focused GameObject and transformArrow png
				std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(focusedObjectID);
				transformArrow->loadFromFile("assets/images/transformArrow.png");
				SDL_Texture* texture = transformArrow->getTexture();
				// * 3 because the texture is so small. If we change the scale, it will change the render starting
				// position, which we don't want. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)transformArrow->getWidth() * 3;
				float arrowHeight = (float)transformArrow->getHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 0, arrowHeight };
				bool _scalesWithZoom = false;

				// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 3);
				AddImageToDrawList(texture, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, 255));
			}
		}

		if (self->HasChildren())
		{
			if (transform != nullptr)
			{
				parentOffset.x += transform->GetPosition().x;
				parentOffset.y += transform->GetPosition().y;
				parentScale.x *= transform->GetScale().x;
				parentScale.y *= transform->GetScale().y;
			}

			for (int c = 0; c < self->GetChildren().size(); c++)
			{
				std::shared_ptr<GameObject> child = FlatEngine::GetObjectById(self->GetChildren()[c]);

				if (child->IsActive())
					RenderSelfThenChildren(child, parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}
	}

	// Helper - Get a value from world/grid position converted into viewport position. Just add - canvas_p0 to get Window coordinates
	float WorldToViewport(float centerPoint, float worldPosition, float zoomFactor, bool _isYCoord)
	{
		std::shared_ptr<Camera> primaryCamera = FlatEngine::GetLoadedScene()->GetPrimaryCamera();
		float scaleToScreenSizeBy = 1;

		// If we're in Release mode, scale the Game Viewport to the window size
		if (FlatEngine::_isDebugMode == true && primaryCamera != nullptr)
			scaleToScreenSizeBy = GAME_VIEWPORT_HEIGHT / primaryCamera->GetHeight() * .05f;

		if (_isYCoord)
			return centerPoint + (-worldPosition * zoomFactor * scaleToScreenSizeBy);
		else
			return centerPoint + (worldPosition * zoomFactor * scaleToScreenSizeBy);
	}
	// Helper - Get a value from viewport position converted into world/grid position.
	ImVec2 ViewportToWorld(ImVec2 viewportPosition)
	{
		std::shared_ptr<Camera> primaryCamera = FlatEngine::GetLoadedScene()->GetPrimaryCamera();
		float zoom = 10;

		if (primaryCamera != nullptr)
			zoom = primaryCamera->GetZoom();

		float xPos = (viewportPosition.x - worldCenterPoint.x) / zoom;
		float yPos = -(viewportPosition.y - worldCenterPoint.y) / zoom;

		return ImVec2(xPos, yPos);
	}

	ImVec2 AddImageToDrawList(SDL_Texture *texture, Vector2 positionInGrid, ImVec2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor)
	{
		// Changing the scale here because sprites are rendering too large and I want them to start off smaller and also keep the default scale value to 1.0f
		Vector2 newScale = Vector2(scale.x * spriteScaleMultiplier, scale.y * spriteScaleMultiplier);

		float scalingXStart = WorldToViewport(relativeCenterPoint.x, positionInGrid.x - (offsetPx.x * newScale.x), zoomMultiplier);
		float scalingYStart = WorldToViewport(relativeCenterPoint.y, -positionInGrid.y - (offsetPx.y * newScale.y), zoomMultiplier);
		float scalingXEnd = WorldToViewport(scalingXStart, textureWidthPx * newScale.x, zoomMultiplier);
		float scalingYEnd = WorldToViewport(scalingYStart, textureHeightPx * newScale.y, zoomMultiplier);

		float unscaledXStart = WorldToViewport(relativeCenterPoint.x, positionInGrid.x, zoomMultiplier) - offsetPx.x * scale.x;
		float unscaledYStart = WorldToViewport(relativeCenterPoint.y, -positionInGrid.y, zoomMultiplier) - offsetPx.y * scale.y;

		ImVec2 renderStart;
		ImVec2 renderEnd;
		ImVec2 UvStart = { 0, 0 };
		ImVec2 UvEnd = { 1, 1 };

		if (_scalesWithZoom)
		{
			renderStart = ImVec2(scalingXStart, scalingYStart);
			renderEnd = ImVec2(scalingXEnd, scalingYEnd);
		}
		else
		{
			renderStart = ImVec2(unscaledXStart, unscaledYStart);
			renderEnd = ImVec2(renderStart.x + textureWidthPx * scale.x, renderStart.y + textureHeightPx * scale.y);
		}

		// Render sprite to viewport
		draw_list->AddImage((void*)texture, renderStart, renderEnd, UvStart, UvEnd, addColor);

		return renderStart;
	}

	void CreateNewAnimationFile(std::string path)
	{
		// Declare file and input stream
		std::ofstream file_obj;

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our gameObject json objects
		json animationObjectsJsonArray;

		// Set animationObjectsJsonArray to empty
		animationObjectsJsonArray.push_back("NULL");

		// Create the GameObjects json object and add the empty array as the content
		json newFileObject = json::object({ {"Animation Properties", animationObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path)
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
		json animationProperties;

		// Create Animation Property Json data object
		json animationName = json::object({
			{ "Name", propertiesObject->animationName },
			{ "Length", propertiesObject->animationLength },
			{ "Loop", propertiesObject->_loop }
		});
		animationProperties.push_back(animationName);


		for (std::shared_ptr<Animation::S_Transform> transformProp : propertiesObject->transformProperties)
		{
			// Declare components array json object for components
			json transformPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "transformInterpType", transformProp->transformInterpType },
				{ "transformSpeed", transformProp->transformSpeed },
				{ "scaleInterpType", transformProp->scaleInterpType },
				{ "scaleSpeed", transformProp->scaleSpeed },
				{ "time", transformProp->time },
				{ "xMove", transformProp->xMove },
				{ "yMove", transformProp->yMove },
				{ "xScale", transformProp->xScale },
				{ "yScale", transformProp->yScale }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			transformPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Transform" },
				{ "Frames", transformPropertiesArray }
			});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Sprite> spriteProp : propertiesObject->spriteProperties)
		{
			// Declare components array json object for components
			json spritePropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "interpType", spriteProp->interpType },
				{ "speed", spriteProp->speed },
				{ "time", spriteProp->time },
				{ "path", spriteProp->path },
				{ "xOffset", spriteProp->xOffset },
				{ "yOffset", spriteProp->yOffset }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			spritePropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Sprite" },
				{ "Frames", spritePropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Camera> cameraProp : propertiesObject->cameraProperties)
		{
			// Declare components array json object for components
			json cameraPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", cameraProp->time },
				{ "_isPrimaryCamera", cameraProp->_isPrimaryCamera }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			cameraPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Camera" },
				{ "Frames", cameraPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Script> scriptProp : propertiesObject->scriptProperties)
		{
			// Declare components array json object for components
			json scriptPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", scriptProp->time },
				{ "path", scriptProp->path }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			scriptPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Script" },
				{ "Frames", scriptPropertiesArray }
			});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Button> buttonProp : propertiesObject->buttonProperties)
		{
			// Declare components array json object for components
			json buttonPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", buttonProp->time },
				{ "_isActive", buttonProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			buttonPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Button" },
				{ "Frames", buttonPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Canvas> canvasProp : propertiesObject->canvasProperties)
		{
			// Declare components array json object for components
			json canvasPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", canvasProp->time }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			canvasPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Canvas" },
				{ "Frames", canvasPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Audio> audioProp : propertiesObject->audioProperties)
		{
			// Declare components array json object for components
			json audioPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", audioProp->time },
				{ "path", audioProp->path },
				{ "_isMusic", audioProp->_isMusic }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			audioPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Audio" },
				{ "Frames", audioPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Text> textProp : propertiesObject->textProperties)
		{
			// Declare components array json object for components
			json textPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", textProp->time },
				{ "path", textProp->path },
				{ "text", textProp->text },
				{ "color", textProp->color }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			textPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Text" },
				{ "Frames", textPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_BoxCollider> boxColliderProp : propertiesObject->boxColliderProperties)
		{
			// Declare components array json object for components
			json boxColliderPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", boxColliderProp->time },
				{ "_isActive", boxColliderProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			boxColliderPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "BoxCollider" },
				{ "Frames", boxColliderPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_CircleCollider> circleColliderProp : propertiesObject->circleColliderProperties)
		{
			// Declare components array json object for components
			json circleColliderPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", circleColliderProp->time },
				{ "_isActive", circleColliderProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			circleColliderPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "CircleCollider" },
				{ "Frames", circleColliderPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_RigidBody> rigidBodyProp : propertiesObject->rigidBodyProperties)
		{
			// Declare components array json object for components
			json rigidBodyPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", rigidBodyProp->time },
				{ "interpType", rigidBodyProp->interpType },
				{ "speed", rigidBodyProp->speed },
				{ "_isActive", rigidBodyProp->_isActive },
				{ "gravityScale", rigidBodyProp->gravityScale },
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			rigidBodyPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "RigidBody" },
				{ "Frames", rigidBodyPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_CharacterController> characterControllerProp : propertiesObject->characterControllerProperties)
		{
			// Declare components array json object for components
			json characterControllerPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", characterControllerProp->time },
				{ "_isActive", characterControllerProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			characterControllerPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "CharacterController" },
				{ "Frames", characterControllerPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"Animation Properties", animationProperties } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path)
	{
		std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
		std::shared_ptr<Animation::S_Transform> transformProperties;
		std::shared_ptr<Animation::S_Sprite > spriteProperties;

		// Save the path to the animationProperties struct
		animationProperties->animationPath = path;

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

			if (fileContentJson["Animation Properties"][0] != "NULL")
			{
				// Getting data from the json 
				// auto properties = fileContentJson["Animation Properties"];
				// std::string name = properties[0]["name"];

				// Set default values
				animationProperties->animationName = "New Animation";

				// Loop through the saved Properties in the JSON file
				for (int i = 0; i < fileContentJson["Animation Properties"].size(); i++)
				{
					// Get data from the loaded object
					json currentObjectJson = fileContentJson["Animation Properties"][i];

					if (currentObjectJson.contains("Name"))
						animationProperties->animationName = currentObjectJson["Name"];
					if (currentObjectJson.contains("Length"))
						animationProperties->animationLength = currentObjectJson["Length"];
					if (currentObjectJson.contains("Loop"))
						animationProperties->_loop = currentObjectJson["Loop"];
					if (currentObjectJson.contains("Property"))
					{
						if (currentObjectJson["Property"] == "Transform")
						{							
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Transform> transformFrames = std::make_shared<Animation::S_Transform>();
									transformFrames->name = "Transform";
									if (currentObjectJson["Frames"][f]["transformInterpType"] == Animation::InterpType::Lerp)
										transformFrames->transformInterpType = Animation::InterpType::Lerp;
									if (currentObjectJson["Frames"][f]["transformInterpType"] == Animation::InterpType::Slerp)
										transformFrames->transformInterpType = Animation::InterpType::Slerp;
									transformFrames->transformSpeed = currentObjectJson["Frames"][f]["transformSpeed"];
									if (currentObjectJson["Frames"][f]["scaleInterpType"] == "Lerp")
										transformFrames->scaleInterpType = Animation::InterpType::Lerp;
									transformFrames->scaleSpeed = currentObjectJson["Frames"][f]["scaleSpeed"];
									transformFrames->time = currentObjectJson["Frames"][f]["time"];
									transformFrames->xMove = currentObjectJson["Frames"][f]["xMove"];
									transformFrames->yMove = currentObjectJson["Frames"][f]["yMove"];
									transformFrames->xScale = currentObjectJson["Frames"][f]["xScale"];
									transformFrames->yScale = currentObjectJson["Frames"][f]["yScale"];

									// Save the data to the animationProperties struct
									animationProperties->transformProperties.push_back(transformFrames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Sprite")
						{						
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Sprite> spriteFrames = std::make_shared<Animation::S_Sprite>();
									spriteFrames->name = "Sprite";
									if (currentObjectJson["Frames"][f]["interpType"] == "Lerp")
										spriteFrames->interpType = Animation::InterpType::Lerp;
									spriteFrames->speed = currentObjectJson["Frames"][f]["speed"];
									spriteFrames->time = currentObjectJson["Frames"][f]["time"];
									spriteFrames->xOffset = currentObjectJson["Frames"][f]["xOffset"];
									spriteFrames->yOffset = currentObjectJson["Frames"][f]["yOffset"];
									spriteFrames->path = currentObjectJson["Frames"][f]["path"];

									// Save the data to the animationProperties struct
									animationProperties->spriteProperties.push_back(spriteFrames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Camera")
						{
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Camera> cameraFrames = std::make_shared<Animation::S_Camera>();
									cameraFrames->name = "Camera";
									cameraFrames->_isPrimaryCamera = currentObjectJson["Frames"][f]["_isPrimaryCamera"];
									cameraFrames->time = currentObjectJson["Frames"][f]["time"];
				
									animationProperties->cameraProperties.push_back(cameraFrames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Script")
						{
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Script> Frames = std::make_shared<Animation::S_Script>();
									Frames->name = "Script";
									Frames->path = currentObjectJson["Frames"][f]["path"];
									Frames->time = currentObjectJson["Frames"][f]["time"];
									
									animationProperties->scriptProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Button")
						{							
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{					
									std::shared_ptr<Animation::S_Button> Frames = std::make_shared<Animation::S_Button>();
									Frames->name = "Button";
									Frames->_isActive = currentObjectJson["Frames"][f]["_isActive"];
									Frames->time = currentObjectJson["Frames"][f]["time"];

									animationProperties->buttonProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Canvas")
						{							
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Canvas> Frames = std::make_shared<Animation::S_Canvas>();
									Frames->name = "Canvas";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									
									animationProperties->canvasProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Audio")
						{							
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Audio> Frames = std::make_shared<Animation::S_Audio>();
									Frames->name = "Audio";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->path = currentObjectJson["Frames"][f]["path"];
									Frames->_isMusic = currentObjectJson["Frames"][f]["_isMusic"];
									
									animationProperties->audioProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "Text")
						{							
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Text> Frames = std::make_shared<Animation::S_Text>();
									Frames->name = "Text";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->path = currentObjectJson["Frames"][f]["path"];
									Frames->text = currentObjectJson["Frames"][f]["text"];
									Frames->color = currentObjectJson["Frames"][f]["color"];
									
									animationProperties->textProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "BoxCollider")
						{						
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_BoxCollider> Frames = std::make_shared<Animation::S_BoxCollider>();
									Frames->name = "BoxCollider";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->_isActive = currentObjectJson["Frames"][f]["_isActive"];
									
									animationProperties->boxColliderProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "CircleCollider")
						{
							// Check Frames key exists
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_CircleCollider> Frames = std::make_shared<Animation::S_CircleCollider>();
									Frames->name = "CircleCollider";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->_isActive = currentObjectJson["Frames"][f]["_isActive"];

									// Save the data to the animationProperties struct
									animationProperties->circleColliderProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "RigidBody")
						{
							// Check Frames key exists
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_RigidBody> Frames = std::make_shared<Animation::S_RigidBody>();
									Frames->name = "RigidBody";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->_isActive = currentObjectJson["Frames"][f]["_isActive"];
									Frames->interpType = currentObjectJson["Frames"][f]["interpType"];
									Frames->speed = currentObjectJson["Frames"][f]["speed"];
									Frames->gravityScale = currentObjectJson["Frames"][f]["gravityScale"];
									
									// Save the data to the animationProperties struct
									animationProperties->rigidBodyProperties.push_back(Frames);
								}
							}
						}
						else if (currentObjectJson["Property"] == "CharacterController")
						{
							// Check Frames key exists
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_CharacterController> Frames = std::make_shared<Animation::S_CharacterController>();
									Frames->name = "CharacterController";
									Frames->time = currentObjectJson["Frames"][f]["time"];
									Frames->_isActive = currentObjectJson["Frames"][f]["_isActive"];

									// Save the data to the animationProperties struct
									animationProperties->characterControllerProperties.push_back(Frames);
								}
							}
						}
					}
				}
			}
		}

		return animationProperties;
	}

	void Cleanup()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}
}
}










//// DirectX12 Helper functions
//bool CreateDeviceD3D(HWND hWnd)
//{
//	// Setup swap chain
//	DXGI_SWAP_CHAIN_DESC1 sd;
//	{
//		ZeroMemory(&sd, sizeof(sd));
//		sd.BufferCount = NUM_BACK_BUFFERS;
//		sd.Width = 0;
//		sd.Height = 0;
//		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
//		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//		sd.Scaling = DXGI_SCALING_STRETCH;
//		sd.Stereo = FALSE;
//	}
//
//	// [DEBUG] Enable debug interface
//	#ifdef DX12_ENABLE_DEBUG_LAYER
//	ID3D12Debug* pdx12Debug = nullptr;
//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
//		pdx12Debug->EnableDebugLayer();
//	#endif
//
//	// Create device
//	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
//		return false;
//
//	// [DEBUG] Setup debug interface to break on any warnings/errors
//	#ifdef DX12_ENABLE_DEBUG_LAYER
//	if (pdx12Debug != nullptr)
//	{
//		ID3D12InfoQueue* pInfoQueue = nullptr;
//		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
//		pInfoQueue->Release();
//		pdx12Debug->Release();
//	}
//	#endif
//
//	{
//		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//		desc.NumDescriptors = NUM_BACK_BUFFERS;
//		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//		desc.NodeMask = 1;
//		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
//			return false;
//
//		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
//		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//		{
//			g_mainRenderTargetDescriptor[i] = rtvHandle;
//			rtvHandle.ptr += rtvDescriptorSize;
//		}
//	}
//
//	{
//		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//		desc.NumDescriptors = 1;
//		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
//			return false;
//	}
//
//	{
//		D3D12_COMMAND_QUEUE_DESC desc = {};
//		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//		desc.NodeMask = 1;
//		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
//			return false;
//	}
//
//	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
//		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
//			return false;
//
//	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
//		g_pd3dCommandList->Close() != S_OK)
//		return false;
//
//	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
//		return false;
//
//	g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	if (g_fenceEvent == nullptr)
//		return false;
//
//	{
//		IDXGIFactory4* dxgiFactory = nullptr;
//		IDXGISwapChain1* swapChain1 = nullptr;
//		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
//			return false;
//		if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
//			return false;
//		if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
//			return false;
//		swapChain1->Release();
//		dxgiFactory->Release();
//		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
//		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
//	}
//
//	CreateRenderTarget();
//	return true;
//}
//
//void CleanupDeviceD3D()
//{
//	CleanupRenderTarget();
//	if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, nullptr); g_pSwapChain->Release(); g_pSwapChain = nullptr; }
//	if (g_hSwapChainWaitableObject != nullptr) { CloseHandle(g_hSwapChainWaitableObject); }
//	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
//		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = nullptr; }
//	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = nullptr; }
//	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
//	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
//	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
//	if (g_fence) { g_fence->Release(); g_fence = nullptr; }
//	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }
//	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
//
//#ifdef DX12_ENABLE_DEBUG_LAYER
//	IDXGIDebug1* pDebug = nullptr;
//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
//	{
//		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
//		pDebug->Release();
//	}
//#endif
//}
//
//void CreateRenderTarget()
//{
//	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//	{
//		ID3D12Resource* pBackBuffer = nullptr;
//		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
//		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
//		g_mainRenderTargetResource[i] = pBackBuffer;
//	}
//}
//
//void CleanupRenderTarget()
//{
//	WaitForLastSubmittedFrame();
//
//	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
//}
//
//void WaitForLastSubmittedFrame()
//{
//	FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];
//
//	UINT64 fenceValue = frameCtx->FenceValue;
//	if (fenceValue == 0)
//		return; // No fence was signaled
//
//	frameCtx->FenceValue = 0;
//	if (g_fence->GetCompletedValue() >= fenceValue)
//		return;
//
//	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
//	WaitForSingleObject(g_fenceEvent, INFINITE);
//}
//
//FrameContext* WaitForNextFrameResources()
//{
//	UINT nextFrameIndex = g_frameIndex + 1;
//	g_frameIndex = nextFrameIndex;
//
//	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
//	DWORD numWaitableObjects = 1;
//
//	FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
//	UINT64 fenceValue = frameCtx->FenceValue;
//	if (fenceValue != 0) // means no fence was signaled
//	{
//		frameCtx->FenceValue = 0;
//		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
//		waitableObjects[1] = g_fenceEvent;
//		numWaitableObjects = 2;
//	}
//
//	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);
//
//	return frameCtx;
//}
//
//}}
//
//
//
////DirectX12 Variables
//
//struct FrameContext
//{
//	ID3D12CommandAllocator* CommandAllocator;
//	UINT64                  FenceValue;
//};
//
//// Data
//static int const                    NUM_FRAMES_IN_FLIGHT = 3;
//static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
//static UINT                         g_frameIndex = 0;
//
//static int const                    NUM_BACK_BUFFERS = 3;
//static ID3D12Device* g_pd3dDevice = nullptr;
//static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
//static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
//static ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
//static ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
//static ID3D12Fence* g_fence = nullptr;
//static HANDLE                       g_fenceEvent = nullptr;
//static UINT64                       g_fenceLastSignaledValue = 0;
//static IDXGISwapChain3* g_pSwapChain = nullptr;
//static HANDLE                       g_hSwapChainWaitableObject = nullptr;
//static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
//static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
//
//// Forward declarations of helper functions
//bool CreateDeviceD3D(HWND hWnd);
//void CleanupDeviceD3D();
//void CreateRenderTarget();
//void CleanupRenderTarget();
//void WaitForLastSubmittedFrame();
//FrameContext* WaitForNextFrameResources();








//DirectX12 Setup

	//SDL_SysWMinfo wmInfo;
	//SDL_VERSION(&wmInfo.version);
	//SDL_GetWindowWMInfo(Window::window, &wmInfo);
	//HWND hwnd = (HWND)wmInfo.info.win.window;


	//// Initialize Direct3D
	//if (!CreateDeviceD3D(hwnd))
	//{
	//	CleanupDeviceD3D();
	//		
	//}

	// Setup Dear ImGui context
//IMGUI_CHECKVERSION();
//ImGui::CreateContext();
//io = ImGui::GetIO(); (void)io;
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//io.ConfigViewportsNoAutoMerge = true;
//io.ConfigViewportsNoTaskBarIcon = true;

// Setup Dear ImGui style
//ImGui::StyleColorsDark();
//ImGui::StyleColorsLight();

// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//ImGuiStyle& style = ImGui::GetStyle();
//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//{
//	style.WindowRounding = 0.0f;
//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//}

// Setup Platform/Renderer backends
//ImGui_ImplSDL2_InitForD3D(Window::window);
//ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
//	DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
//	g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
//	g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());






//DirectX12 Cleanup

	//WaitForLastSubmittedFrame();

	//// Cleanup
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplSDL2_Shutdown();
	//ImGui::DestroyContext();

	//CleanupDeviceD3D();







//DirectX12 Render

	//SDL_Event event;
	//while (SDL_PollEvent(&event))
	//{
	//	ImGui_ImplSDL2_ProcessEvent(&event);
	//	if (event.type == SDL_QUIT)
	//		quit = true;
	//	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
	//		quit = true;
	//	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(Window::window))
	//	{
	//		// Release all outstanding references to the swap chain's buffers before resizing.
	//		CleanupRenderTarget();
	//		g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//		CreateRenderTarget();
	//	}
	//}

		//// Start the Dear ImGui frame
	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplSDL2_NewFrame();
	//ImGui::NewFrame();

//bool show_demo_window = true;
//	if (show_demo_window)
//	ImGui::ShowDemoWindow(&show_demo_window);


	//// Rendering
	//ImGui::Render();

	//FrameContext* frameCtx = WaitForNextFrameResources();
	//UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
	//frameCtx->CommandAllocator->Reset();

	//D3D12_RESOURCE_BARRIER barrier = {};
	//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
	//g_pd3dCommandList->ResourceBarrier(1, &barrier);
	//TextureManager::RenderTextures();
	//// Render Dear ImGui graphics
	//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	//g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
	//g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
	//g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//g_pd3dCommandList->ResourceBarrier(1, &barrier);
	//g_pd3dCommandList->Close();

	//g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

	// Update and Render additional Platform Windows
	//ImGui::UpdatePlatformWindows();
	//ImGui::RenderPlatformWindowsDefault(nullptr, (void*)g_pd3dCommandList);

	//g_pSwapChain->Present(1, 0); // Present with vsync
	////g_pSwapChain->Present(0, 0); // Present without vsync

	//UINT64 fenceValue = g_fenceLastSignaledValue + 1;
	//g_pd3dCommandQueue->Signal(g_fence, fenceValue);
	//g_fenceLastSignaledValue = fenceValue;
	//frameCtx->FenceValue = fenceValue;




// DirectX12 Handle Events
//void HandleEvents(SDL_Event event, bool& quit)
//{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	//ImGui_ImplSDL2_ProcessEvent(&event);
	//if (event.type == SDL_QUIT)
	//	quit = true;
	//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
	//	quit = true;
	//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(Window::window))
	//{
	//	// Release all outstanding references to the swap chain's buffers before resizing.
	//	CleanupRenderTarget();
	//	g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//	CreateRenderTarget();
	//}
//}