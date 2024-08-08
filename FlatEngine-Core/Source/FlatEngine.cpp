#include "FlatEngine.h"
#include "UIManager.h"
#include "MappingContext.h"
#include "PrefabManager.h"
#include "Logger.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "Window.h"
#include "ECSManager.h"
#include "Project.h"

#include "GameLoop.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "BoxCollider.h"
#include "ScriptComponent.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "Text.h"
#include "Camera.h"
#include "Audio.h"
#include "Sound.h"
#include "Animation.h"

#include <SDL_ttf.h>
#include <SDL.h>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <shobjidl.h> 
#include "implot.h"
#include "imgui_internal.h"


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{
	std::shared_ptr<Application> F_Application = std::make_shared<Application>();

	bool _isDebugMode = true;
	bool _closeProgram = false;

	std::vector<SDL_Joystick*> gamepads = std::vector<SDL_Joystick*>();
	int JOYSTICK_DEAD_ZONE = 4000;

	// For rendering sprites
	int F_maxSpriteLayers = 55;
	float F_spriteScaleMultiplier = 0.2f;

	GameObject* F_PlayerObject = nullptr;
	Project F_LoadedProject = Project();

	// Managers
	Logger F_Logger = Logger();
	UIManager F_UIManager = UIManager();
	SceneManager F_SceneManager = SceneManager();	
	Sound F_SoundController = Sound();
	std::vector<MappingContext> F_MappingContexts = std::vector<MappingContext>();
	TTF_Font* F_fontCinzel;
	std::shared_ptr<PrefabManager> prefabManager = std::make_shared<PrefabManager>();

	// Animator
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = std::make_shared<Animation::S_AnimationProperties>();
	std::shared_ptr<GameObject> objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	int previewAnimationStartTime = 0;
	int previewAnimationTime = 0;
	bool _playPreviewAnimation = true;



	bool LoadFonts()
	{
		bool b_success = true;
		F_fontCinzel = TTF_OpenFont("C:/Users/Dillon Kyle/source/repos/FlatEngine3D/FlatEngine-Core/Source/assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
		if (F_fontCinzel == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			b_success = false;
		}

		return b_success;
	}

	void FreeFonts()
	{
		//Free global fonts
		TTF_CloseFont(F_fontCinzel);
		F_fontCinzel = NULL;
	}


	Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation, ImU32 addColor)
	{
		// Changing the scale here because sprites are rendering too large and I want them to start off smaller and also keep the default scale value to 1.0f
		Vector2 newScale = Vector2(scale.x * F_spriteScaleMultiplier, scale.y * F_spriteScaleMultiplier);

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

			// FOR DEBUGGING - draw white box around where the texture should be
			//FlatEngine::DrawRectangle(renderStart, renderEnd, Vector2(0,0), Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), FlatEngine::F_whiteColor, 2, draw_list);
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


	bool Init(int windowWidth, int windowHeight)
	{
		//Initialization flag
		bool success = true;

		//Initialize SDL
		SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			FlatEngine::LogString("SDL initialized... - Video - Audio - Joystick -");

			//Set texture filtering to linear
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
			{
				printf("Warning: Linear texture filtering not enabled!");
			}

			char t[] = "FlatEngine";
			char* title = &(t[0]);

			//Initialize Window::window
			if (Window::Init(title, windowWidth, windowHeight))
			{
				FlatEngine::LogString("Window initialized...");

				//Initialize SDL_image for png loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					FlatEngine::LogString("SDL_image initialized...");
					//Initialize SDL_ttf for text rendering
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
					else
					{
						FlatEngine::LogString("TTF_Fonts initialized...");
						//Initialize SDL_mixer
						if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
						{
							printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
							success = false;
						}
						else
						{
							SetupImGui(); // Set up ImGui Context and global styles
							CreateIcons(); // Create texture icons
							Mix_AllocateChannels(100); // Sets number of individual audios that can play at once

							FlatEngine::LogString("SDL_mixer initialized...");
							FlatEngine::LogString("Ready...");
							FlatEngine::LogSeparator();
							FlatEngine::LogString("Begin Logging...");
							FlatEngine::LogSeparator();
						}
					}
				}
			}
		}

		return success;
	}

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

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowPadding = { 5.0f, 5.0f };
		style.DockingSeparatorSize = 1;
		style.SeparatorTextAlign = Vector2(0.5f, 0.0f);
		style.SeparatorTextBorderSize = 1;

		ImGui_ImplSDL2_InitForSDLRenderer(Window::W_Window, Window::W_Renderer);
		ImGui_ImplSDLRenderer2_Init(Window::W_Renderer);

		// Round about way of editing the active titlebgactive color since pushstylecolor doesn't seem to work for it.
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName(i);
			//LogString(name);
			if (name == "TitleBgActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_titleBgActiveColor;
			}
			if (name == "TabUnfocusedActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_tabUnfocusedActiveColor;
			}
			if (name == "TabActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_tabActiveColor;
			}
			if (name == "ResizeGripHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_resizeGripHoveredColor;
			}
			if (name == "ResizeGripActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_resizeGripActiveColor;
			}
			if (name == "DockingPreview")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_dockingPreviewColor;
			}
			if (name == "DockingEmptyBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = F_dockingPreviewEmptyColor;
			}
		}
	}

	void ManageControllers()
	{
		static int controllersConnected = 0;
		if (SDL_NumJoysticks() != controllersConnected)
		{
			// Clean up old gamepads
			for (SDL_Joystick* gamepad : gamepads)
			{
				SDL_JoystickClose(gamepad);
				gamepad = NULL;
			}

			controllersConnected = SDL_NumJoysticks();
			for (int i = 0; i < controllersConnected; i++)
			{
				SDL_Joystick* gamepad = SDL_JoystickOpen(i);
				if (gamepad == NULL)
					printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
				else
				{
					LogString("Gamepad connected...");
					gamepads.push_back(gamepad);
				}
			}
		}
	}


	void QuitImGui()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void CloseProgram()
	{
		QuitImGui();

		// Clean up old gamepads
		for (SDL_Joystick* gamepad : gamepads)
		{
			SDL_JoystickClose(gamepad);
			gamepad = NULL;
		}

		FreeFonts();

		//Quit SDL subsystems
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	
		FlatEngine::_closeProgram = true;
	}

	Uint32 GetEngineTime()
	{
		return (int)SDL_GetTicks();
	}

	json LoadFileData(std::string filename)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filename);

		// Open file in in mode
		file_obj.open(filename, std::ios::in);

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

		if (file_obj.good() && fileContent != "\n")
		{
			// Go from string to json object
			return json::parse(fileContent);
		}
		else
			return NULL;
	}


	GameObject* GetPlayerObject()
	{
		return F_PlayerObject;
	}

	// Project Management
	void SetLoadedProject(Project loadedProject)
	{
		F_LoadedProject = loadedProject;
	}

	Project& GetLoadedProject()
	{
		return F_LoadedProject;
	}

	// GameObject / Scene management
	void SaveScene(Scene* scene, std::string filepath)
	{
		F_SceneManager.SaveScene(scene, filepath);
	}

	void SaveCurrentScene()
	{
		F_SceneManager.SaveCurrentScene();
	}

	void LoadScene(std::string filepath)
	{
		// Stop any playing music
		F_SoundController.StopMusic();
		// Reset buttons in UIManager
		F_UIManager.ResetButtons();

		if (F_SceneManager.LoadScene(filepath))
		{
			if (F_SceneManager.GetLoadedScene() != nullptr)
				F_SceneManager.SaveAnimationPreviewObjects();

			F_SceneManager.LoadAnimationPreviewObjects();

			// If the GameLoop is running, reinitialize the new scene's GameObjects
			if (FlatEngine::GameLoopStarted())
				GetLoadedScene()->InitializeScriptObjects(); // Empty function may not need

			F_PlayerObject = GetObjectByTag("Player");
		}
	}

	long GetNextComponentID()
	{
		long nextID = -1;
		Scene* loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextComponentID();
			GetLoadedScene()->IncrementComponentID();
		}

		return nextID;
	}

	long GetNextGameObjectID()
	{
		long nextID = -1;
		Scene* loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextGameObjectID();
		}

		return nextID;
	}

	Scene *GetLoadedScene()
	{
		return F_SceneManager.GetLoadedScene();
	}

	Scene *CreateNewScene()
	{
		return F_SceneManager.CreateNewScene();
	}

	std::string GetLoadedScenePath()
	{
		return F_SceneManager.GetLoadedScenePath();
	}

	std::map<long, GameObject> &GetSceneObjects()
	{
		return GetLoadedScene()->GetSceneObjects();
	}

	GameObject* CreateGameObject(long parentID)
	{
		return GetLoadedScene()->CreateGameObject(parentID);
	}

	void DeleteGameObject(int sceneObjectID)
	{
		GetLoadedScene()->DeleteGameObject(sceneObjectID);
	}

	Component* GetObjectComponent(long objectID, ComponentTypes type)
	{
		return GetLoadedScene()->GetObjectById(objectID)->GetComponent(type);
	}

	GameObject* GetObjectById(long objectID)
	{
		return GetLoadedScene()->GetObjectById(objectID);
	}

	GameObject* GetObjectByName(std::string name)
	{
		return GetLoadedScene()->GetObjectByName(name);
	}

	GameObject* GetObjectByTag(std::string tag)
	{
		return GetLoadedScene()->GetObjectByTag(tag);
	}

	// Mapping Context Management
	void SaveMappingContext(std::string path, MappingContext context)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our mappings json objects
		json mappings = json::array();

		if (context.GetKeyBindings().size() > 0)
		{
			std::string data = context.GetData();
			mappings.push_back(json::parse(data));
		}
		else
			mappings.push_back("NULL");

		// Recreate the json object and add the array as the content
		json newFileObject = json::object({ {"Mapping Context", mappings } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void InitializeMappingContexts()
	{
		F_MappingContexts.clear();

		std::string path = "C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\FlatEngine-Editor\\Source\\mappingContext";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			// Create a new context to save the loaded keybindings to
			MappingContext newContext = MappingContext();

			json contextData = LoadFileData(entry.path().string());
			if (contextData != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto mappings = contextData["Mapping Context"][0];
				
				newContext.SetName(mappings["name"]);
				newContext.SetPath(entry.path().string());
				
				// XInput
				newContext.AddKeyBinding("XInput_A", mappings["XInput_A"]);
				newContext.AddKeyBinding("XInput_B", mappings["XInput_B"]);
				newContext.AddKeyBinding("XInput_X", mappings["XInput_X"]);
				newContext.AddKeyBinding("XInput_Y", mappings["XInput_Y"]);
				newContext.AddKeyBinding("XInput_LB", mappings["XInput_LB"]);
				newContext.AddKeyBinding("XInput_RB", mappings["XInput_RB"]);
				newContext.AddKeyBinding("XInput_ScreenShot", mappings["XInput_ScreenShot"]);
				newContext.AddKeyBinding("XInput_Start", mappings["XInput_Start"]);
				newContext.AddKeyBinding("XInput_LS", mappings["XInput_LS"]);
				newContext.AddKeyBinding("XInput_RS", mappings["XInput_RS"]);
				newContext.AddKeyBinding("XInput_Home", mappings["XInput_Home"]);
				newContext.AddKeyBinding("XInput_Tray", mappings["XInput_Tray"]);
				newContext.AddKeyBinding("XInput_DPadUp", mappings["XInput_DPadUp"]);
				newContext.AddKeyBinding("XInput_DPadDown", mappings["XInput_DPadDown"]);
				newContext.AddKeyBinding("XInput_DPadLeft", mappings["XInput_DPadLeft"]);
				newContext.AddKeyBinding("XInput_DPadRight", mappings["XInput_DPadRight"]);
				newContext.AddKeyBinding("XInput_LeftJoystickX", mappings["XInput_LeftJoystickX"]);
				newContext.AddKeyBinding("XInput_LeftJoystickY", mappings["XInput_LeftJoystickY"]);
				newContext.AddKeyBinding("XInput_RightJoystick", mappings["XInput_RightJoystick"]);
				newContext.AddKeyBinding("XInput_LT", mappings["XInput_LT"]);
				newContext.AddKeyBinding("XInput_RT", mappings["XInput_RT"]);

				//// Keyboard + Mouse
				// Directional
				newContext.AddKeyBinding("SDLK_UP", mappings["SDLK_UP"]);
				newContext.AddKeyBinding("SDLK_DOWN", mappings["SDLK_DOWN"]);
				newContext.AddKeyBinding("SDLK_LEFT", mappings["SDLK_LEFT"]);
				newContext.AddKeyBinding("SDLK_RIGHT", mappings["SDLK_RIGHT"]);
				// Letters
				newContext.AddKeyBinding("SDLK_a", mappings["SDLK_a"]);
				newContext.AddKeyBinding("SDLK_b", mappings["SDLK_b"]);
				newContext.AddKeyBinding("SDLK_c", mappings["SDLK_c"]);
				newContext.AddKeyBinding("SDLK_d", mappings["SDLK_d"]);
				newContext.AddKeyBinding("SDLK_e", mappings["SDLK_e"]);
				newContext.AddKeyBinding("SDLK_f", mappings["SDLK_f"]);
				newContext.AddKeyBinding("SDLK_g", mappings["SDLK_g"]);
				newContext.AddKeyBinding("SDLK_h", mappings["SDLK_h"]);
				newContext.AddKeyBinding("SDLK_i", mappings["SDLK_i"]);
				newContext.AddKeyBinding("SDLK_j", mappings["SDLK_j"]);
				newContext.AddKeyBinding("SDLK_k", mappings["SDLK_k"]);
				newContext.AddKeyBinding("SDLK_l", mappings["SDLK_l"]);
				newContext.AddKeyBinding("SDLK_m", mappings["SDLK_m"]);
				newContext.AddKeyBinding("SDLK_n", mappings["SDLK_n"]);
				newContext.AddKeyBinding("SDLK_o", mappings["SDLK_o"]);
				newContext.AddKeyBinding("SDLK_p", mappings["SDLK_p"]);
				newContext.AddKeyBinding("SDLK_q", mappings["SDLK_q"]);
				newContext.AddKeyBinding("SDLK_r", mappings["SDLK_r"]);
				newContext.AddKeyBinding("SDLK_s", mappings["SDLK_s"]);
				newContext.AddKeyBinding("SDLK_t", mappings["SDLK_t"]);
				newContext.AddKeyBinding("SDLK_u", mappings["SDLK_u"]);
				newContext.AddKeyBinding("SDLK_v", mappings["SDLK_v"]);
				newContext.AddKeyBinding("SDLK_w", mappings["SDLK_w"]);
				newContext.AddKeyBinding("SDLK_x", mappings["SDLK_x"]);
				newContext.AddKeyBinding("SDLK_y", mappings["SDLK_y"]);
				newContext.AddKeyBinding("SDLK_z", mappings["SDLK_z"]);

				// After all keys are set, create their Input Action bindings
				newContext.CreateInputActionBindings();

				// Add context to context managing vector
				F_MappingContexts.push_back(newContext);
			}
		}
	}

	MappingContext* GetMappingContext(std::string contextName)
	{
		for (std::vector<MappingContext>::iterator iter = F_MappingContexts.begin(); iter != F_MappingContexts.end();)
		{
			if (iter->GetName() == contextName)
				return &(*iter);

			iter++;
		}

		return nullptr;
	}

	void HandleEvents(bool& quit)
	{
		//std::vector<FlatEngine::MappingContext> *FlatEngine::F_MappingContexts = &FlatEngine::F_FlatEngine::F_MappingContexts;
		using XInputAxis = FlatEngine::XInputAxis;
		using XInputButtons = FlatEngine::XInputButtons;
		using XInputHats = FlatEngine::XInputHats;
		int JOYSTICK_DEAD_ZONE = FlatEngine::JOYSTICK_DEAD_ZONE;

		// Unfire all keybinds that were fired in the last frame then clear the saved keys
		static std::vector<std::string> firedKeys = std::vector<std::string>();
		for (std::string keybind : firedKeys)
			for (FlatEngine::MappingContext mappingContext : FlatEngine::F_MappingContexts)
				mappingContext.UnFireEvent(keybind);
		firedKeys.clear();


		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				quit = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::W_Window))
				quit = true;

			for (MappingContext context : F_MappingContexts)
			{
				HandleContextEvents(context, event, firedKeys);
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

	void HandleContextEvents(MappingContext& context, SDL_Event event, std::vector<std::string> &firedKeys)
	{
		// Keyboard Keys Down
		if (event.type == SDL_KEYDOWN)
		{
			// Send event to context inputAction
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_SPACE") != "" && mappingContext.GetKeyBoundEvent("SDLK_SPACE").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_SPACE", event);
						firedKeys.push_back("SDLK_SPACE");
					}
				break;

			case SDLK_UP:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_UP") != "" && mappingContext.GetKeyBoundEvent("SDLK_UP").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_UP", event);
						firedKeys.push_back("SDLK_UP");
					}
				break;

			case SDLK_DOWN:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_DOWN") != "" && mappingContext.GetKeyBoundEvent("SDLK_DOWN").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_DOWN", event);
						firedKeys.push_back("SDLK_DOWN");
					}
				break;

			case SDLK_LEFT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_LEFT") != "" && mappingContext.GetKeyBoundEvent("SDLK_LEFT").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_LEFT", event);
						firedKeys.push_back("SDLK_LEFT");
					}
				break;

			case SDLK_RIGHT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_RIGHT") != "" && mappingContext.GetKeyBoundEvent("SDLK_RIGHT").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_RIGHT", event);
						firedKeys.push_back("SDLK_RIGHT");
					}
				break;

			case SDLK_a:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_a") != "" && mappingContext.GetKeyBoundEvent("SDLK_a").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_a", event);
						firedKeys.push_back("SDLK_a");
					}
				break;

			case SDLK_b:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_b") != "" && mappingContext.GetKeyBoundEvent("SDLK_b").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_b", event);
						firedKeys.push_back("SDLK_b");
					}
				break;

			case SDLK_c:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_c") != "" && mappingContext.GetKeyBoundEvent("SDLK_c").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_c", event);
						firedKeys.push_back("SDLK_c");
					}
				break;

			case SDLK_d:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_d") != "" && mappingContext.GetKeyBoundEvent("SDLK_d").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_d", event);
						firedKeys.push_back("SDLK_d");
					}
				break;

			case SDLK_e:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_e") != "" && mappingContext.GetKeyBoundEvent("SDLK_e").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_e", event);
						firedKeys.push_back("SDLK_e");
					}
				break;

			case SDLK_f:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_f") != "" && mappingContext.GetKeyBoundEvent("SDLK_f").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_f", event);
						firedKeys.push_back("SDLK_f");
					}
				break;

			case SDLK_g:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_g") != "" && mappingContext.GetKeyBoundEvent("SDLK_g").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_g", event);
						firedKeys.push_back("SDLK_g");
					}
				break;

			case SDLK_h:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_h") != "" && mappingContext.GetKeyBoundEvent("SDLK_h").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_h", event);
						firedKeys.push_back("SDLK_h");
					}
				break;

			case SDLK_i:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_i") != "" && mappingContext.GetKeyBoundEvent("SDLK_i").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_i", event);
						firedKeys.push_back("SDLK_i");
					}
				break;

			case SDLK_j:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_j") != "" && mappingContext.GetKeyBoundEvent("SDLK_j").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_j", event);
						firedKeys.push_back("SDLK_j");
					}
				break;

			case SDLK_k:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_k") != "" && mappingContext.GetKeyBoundEvent("SDLK_k").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_k", event);
						firedKeys.push_back("SDLK_k");
					}
				break;

			case SDLK_l:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_l") != "" && mappingContext.GetKeyBoundEvent("SDLK_l").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_l", event);
						firedKeys.push_back("SDLK_l");
					}
				break;

			case SDLK_m:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_m") != "" && mappingContext.GetKeyBoundEvent("SDLK_m").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_m", event);
						firedKeys.push_back("SDLK_m");
					}
				break;

			case SDLK_n:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_n") != "" && mappingContext.GetKeyBoundEvent("SDLK_n").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_n", event);
						firedKeys.push_back("SDLK_n");
					}
				break;

			case SDLK_o:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_o") != "" && mappingContext.GetKeyBoundEvent("SDLK_o").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_o", event);
						firedKeys.push_back("SDLK_o");
					}
				break;

			case SDLK_p:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_p") != "" && mappingContext.GetKeyBoundEvent("SDLK_p").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_p", event);
						firedKeys.push_back("SDLK_p");
					}
				break;

			case SDLK_q:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_q") != "" && mappingContext.GetKeyBoundEvent("SDLK_q").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_q", event);
						firedKeys.push_back("SDLK_q");
					}
				break;

			case SDLK_r:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_r") != "" && mappingContext.GetKeyBoundEvent("SDLK_r").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_r", event);
						firedKeys.push_back("SDLK_r");
					}
				break;

			case SDLK_s:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_s") != "" && mappingContext.GetKeyBoundEvent("SDLK_s").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_s", event);
						firedKeys.push_back("SDLK_s");
					}
				break;

			case SDLK_t:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_t") != "" && mappingContext.GetKeyBoundEvent("SDLK_t").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_t", event);
						firedKeys.push_back("SDLK_t");
					}
				break;

			case SDLK_u:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_u") != "" && mappingContext.GetKeyBoundEvent("SDLK_u").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_u", event);
						firedKeys.push_back("SDLK_u");
					}
				break;

			case SDLK_v:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_v") != "" && mappingContext.GetKeyBoundEvent("SDLK_v").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_v", event);
						firedKeys.push_back("SDLK_v");
					}
				break;

			case SDLK_w:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_w") != "" && mappingContext.GetKeyBoundEvent("SDLK_w").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_w", event);
						firedKeys.push_back("SDLK_w");
					}
				break;

			case SDLK_x:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_x") != "" && mappingContext.GetKeyBoundEvent("SDLK_x").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_x", event);
						firedKeys.push_back("SDLK_x");
					}
				break;

			case SDLK_y:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_y") != "" && mappingContext.GetKeyBoundEvent("SDLK_y").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_y", event);
						firedKeys.push_back("SDLK_y");
					}
				break;

			case SDLK_z:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_z") != "" && mappingContext.GetKeyBoundEvent("SDLK_z").type == 0)
					{
						mappingContext.OnInputEvent("SDLK_z", event);
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
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_SPACE") != "")
						mappingContext.ClearInputActionEvent("SDLK_SPACE");
				break;

			case SDLK_UP:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_UP") != "")
						mappingContext.ClearInputActionEvent("SDLK_UP");
				break;

			case SDLK_DOWN:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_DOWN") != "")
						mappingContext.ClearInputActionEvent("SDLK_DOWN");
				break;

			case SDLK_LEFT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_LEFT") != "")
					{
						mappingContext.ClearInputActionEvent("SDLK_LEFT");
						mappingContext.UnFireEvent("SDLK_LEFT");
					}

				break;

			case SDLK_RIGHT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_RIGHT") != "")
						mappingContext.ClearInputActionEvent("SDLK_RIGHT");
				break;

			case SDLK_a:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_a") != "")
						mappingContext.ClearInputActionEvent("SDLK_a");
				break;

			case SDLK_b:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_b") != "")
						mappingContext.ClearInputActionEvent("SDLK_b");
				break;

			case SDLK_c:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_c") != "")
						mappingContext.ClearInputActionEvent("SDLK_c");
				break;

			case SDLK_d:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_d") != "")
						mappingContext.ClearInputActionEvent("SDLK_d");
				break;

			case SDLK_e:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_e") != "")
						mappingContext.ClearInputActionEvent("SDLK_e");
				break;

			case SDLK_f:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_f") != "")
						mappingContext.ClearInputActionEvent("SDLK_f");
				break;

			case SDLK_g:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_g") != "")
						mappingContext.ClearInputActionEvent("SDLK_g");
				break;

			case SDLK_h:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_h") != "")
						mappingContext.ClearInputActionEvent("SDLK_h");
				break;

			case SDLK_i:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_i") != "")
						mappingContext.ClearInputActionEvent("SDLK_i");
				break;

			case SDLK_j:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_j") != "")
						mappingContext.ClearInputActionEvent("SDLK_j");
				break;

			case SDLK_k:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_k") != "")
						mappingContext.ClearInputActionEvent("SDLK_k");
				break;

			case SDLK_l:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_l") != "")
						mappingContext.ClearInputActionEvent("SDLK_l");
				break;

			case SDLK_m:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_m") != "")
						mappingContext.ClearInputActionEvent("SDLK_m");
				break;

			case SDLK_n:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_n") != "")
						mappingContext.ClearInputActionEvent("SDLK_n");
				break;

			case SDLK_o:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_o") != "")
						mappingContext.ClearInputActionEvent("SDLK_o");
				break;

			case SDLK_p:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_p") != "")
						mappingContext.ClearInputActionEvent("SDLK_p");
				break;

			case SDLK_q:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_q") != "")
						mappingContext.ClearInputActionEvent("SDLK_q");
				break;

			case SDLK_r:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_r") != "")
						mappingContext.ClearInputActionEvent("SDLK_r");
				break;

			case SDLK_s:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_s") != "")
						mappingContext.ClearInputActionEvent("SDLK_s");
				break;

			case SDLK_t:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_t") != "")
						mappingContext.ClearInputActionEvent("SDLK_t");
				break;

			case SDLK_u:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_u") != "")
						mappingContext.ClearInputActionEvent("SDLK_u");
				break;

			case SDLK_v:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_v") != "")
						mappingContext.ClearInputActionEvent("SDLK_v");
				break;

			case SDLK_w:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_w") != "")
						mappingContext.ClearInputActionEvent("SDLK_w");
				break;

			case SDLK_x:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_x") != "")
						mappingContext.ClearInputActionEvent("SDLK_x");
				break;

			case SDLK_y:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_y") != "")
						mappingContext.ClearInputActionEvent("SDLK_y");
				break;

			case SDLK_z:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("SDLK_z") != "")
						mappingContext.ClearInputActionEvent("SDLK_z");
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
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LeftJoystickX") != "")
						mappingContext.OnInputEvent("XInput_LeftJoystickX", event);
				break;
			case XInputAxis::LeftYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LeftJoystickY") != "")
						mappingContext.OnInputEvent("XInput_LeftJoystickY", event);
				break;
			case XInputAxis::RightXAxis:
				// Left of dead zone or Right of dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RightJoystick") != "")
						mappingContext.OnInputEvent("XInput_RightJoystick", event);
				break;
			case XInputAxis::RightYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RightJoystick") != "")
						mappingContext.OnInputEvent("XInput_RightJoystick", event);
				break;
			case XInputAxis::LT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LT") != "")
						mappingContext.OnInputEvent("XInput_LT", event);
				break;
			case XInputAxis::RT:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RT") != "")
						mappingContext.OnInputEvent("XInput_RT", event);
				break;
			}
		}
		// Buttons Down
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			switch (event.jbutton.button)
			{
			case XInputButtons::A:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_A") != "")
					{
						mappingContext.OnInputEvent("XInput_A", event);
						firedKeys.push_back("XInput_A");
					}

				break;
			case XInputButtons::B:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_B") != "")
					{
						mappingContext.OnInputEvent("XInput_B", event);
						firedKeys.push_back("XInput_B");
					}
				break;
			case XInputButtons::X:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_X") != "")
					{
						mappingContext.OnInputEvent("XInput_X", event);
						firedKeys.push_back("XInput_X");
					}
				break;
			case XInputButtons::Y:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Y") != "")
					{
						mappingContext.OnInputEvent("XInput_Y", event);
						firedKeys.push_back("XInput_Y");
					}
				break;
			case XInputButtons::LB:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LB") != "")
					{
						mappingContext.OnInputEvent("XInput_LB", event);
						firedKeys.push_back("XInput_LB");
					}
				break;
			case XInputButtons::RB:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RB") != "")
					{
						mappingContext.OnInputEvent("XInput_RB", event);
						firedKeys.push_back("XInput_RB");
					}
				break;
			case XInputButtons::ScreenShot:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_ScreenShot") != "")
					{
						mappingContext.OnInputEvent("XInput_ScreenShot", event);
						firedKeys.push_back("XInput_ScreenShot");
					}
				break;
			case XInputButtons::Start:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Start") != "")
					{
						mappingContext.OnInputEvent("XInput_Start", event);
						firedKeys.push_back("XInput_Start");
					}
				break;
			case XInputButtons::LS:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LS") != "")
					{
						mappingContext.OnInputEvent("XInput_LS", event);
						firedKeys.push_back("XInput_LS");
					}
				break;
			case XInputButtons::RS:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RS") != "")
					{
						mappingContext.OnInputEvent("XInput_RS", event);
						firedKeys.push_back("XInput_RS");
					}
				break;
			case XInputButtons::Home:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Home") != "")
					{
						mappingContext.OnInputEvent("XInput_Home", event);
						firedKeys.push_back("XInput_Home");
					}
				break;
			case XInputButtons::Tray:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Tray") != "")
					{
						mappingContext.OnInputEvent("XInput_Tray", event);
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
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_A") != "")
						mappingContext.ClearInputActionEvent("XInput_A");
				break;
			case XInputButtons::B:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_B") != "")
						mappingContext.ClearInputActionEvent("XInput_B");
				break;
			case XInputButtons::X:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_X") != "")
						mappingContext.ClearInputActionEvent("XInput_X");
				break;
			case XInputButtons::Y:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Y") != "")
						mappingContext.ClearInputActionEvent("XInput_Y");
				break;
			case XInputButtons::LB:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LB") != "")
						mappingContext.ClearInputActionEvent("XInput_LB");
				break;
			case XInputButtons::RB:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RB") != "")
						mappingContext.ClearInputActionEvent("XInput_RB");
				break;
			case XInputButtons::ScreenShot:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_ScreenShot") != "")
						mappingContext.ClearInputActionEvent("XInput_ScreenShot");
				break;
			case XInputButtons::Start:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Start") != "")
						mappingContext.ClearInputActionEvent("XInput_Start");
				break;
			case XInputButtons::LS:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_LS") != "")
						mappingContext.ClearInputActionEvent("XInput_LS");
				break;
			case XInputButtons::RS:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_RS") != "")
						mappingContext.ClearInputActionEvent("XInput_RS");
				break;
			case XInputButtons::Home:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Home") != "")
						mappingContext.ClearInputActionEvent("XInput_Home");
				break;
			case XInputButtons::Tray:
				for (MappingContext mappingContext : FlatEngine::F_MappingContexts)
					if (mappingContext.GetKeyBinding("XInput_Tray") != "")
						mappingContext.ClearInputActionEvent("XInput_Tray");
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


	// Json Parsing
	json CreateJsonFromObject(GameObject currentObject)
	{
		// Declare components array json object for components
		json componentsArray = json::array();

		std::vector<Component*> components = currentObject.GetComponents();

		for (int j = 0; j < components.size(); j++)
		{
			std::string typeString = components[j]->GetTypeString();

			if (typeString != "Null")
			{
				std::string data = components[j]->GetData();
				componentsArray.push_back(json::parse(data));
			}
		}

		// Declare children array json object for children
		json childrenArray = json::array();

		for (int c = 0; c < currentObject.GetChildren().size(); c++)
		{
			childrenArray.push_back(currentObject.GetChildren()[c]);
		}

		// Declare tags object for GameObject Tags	
		std::map <std::string, bool> tagList = currentObject.GetTagList().GetTagsMap();
		std::map <std::string, bool> ignoreTagList = currentObject.GetTagList().GetIgnoreTagsMap();

		json tagsObjectJson = json::object({
			{ "Player", tagList.at("Player") },
			{ "Enemy", tagList.at("Enemy") },
			{ "Npc", tagList.at("Npc") },
			{ "Terrain", tagList.at("Terrain") },
			{ "PlayerTrigger", tagList.at("PlayerTrigger") },
			{ "EnemyTrigger", tagList.at("EnemyTrigger") },
			{ "NpcTrigger", tagList.at("NpcTrigger") },
			{ "EnvironmentalTrigger", tagList.at("EnvironmentalTrigger") },
			{ "TerrainTrigger", tagList.at("TerrainTrigger") },
			{ "PlayerDamage", tagList.at("PlayerDamage") },
			{ "EnemyDamage", tagList.at("EnemyDamage") },
			{ "EnvironmentalDamage", tagList.at("EnvironmentalDamage") },
			{ "Projectile", tagList.at("Projectile") },
			{ "InteractableItem", tagList.at("InteractableItem") },
			{ "InteractableObject", tagList.at("InteractableObject") },
			{ "Item", tagList.at("Item") },
			});

		json ignoreTagsObjectJson = json::object({
			{ "Player", ignoreTagList.at("Player") },
			{ "Enemy", ignoreTagList.at("Enemy") },
			{ "Npc", ignoreTagList.at("Npc") },
			{ "Terrain", ignoreTagList.at("Terrain") },
			{ "PlayerTrigger", ignoreTagList.at("PlayerTrigger") },
			{ "EnemyTrigger", ignoreTagList.at("EnemyTrigger") },
			{ "NpcTrigger", ignoreTagList.at("NpcTrigger") },
			{ "EnvironmentalTrigger", ignoreTagList.at("EnvironmentalTrigger") },
			{ "TerrainTrigger", ignoreTagList.at("TerrainTrigger") },
			{ "PlayerDamage", ignoreTagList.at("PlayerDamage") },
			{ "EnemyDamage", ignoreTagList.at("EnemyDamage") },
			{ "EnvironmentalDamage", ignoreTagList.at("EnvironmentalDamage") },
			{ "Projectile", ignoreTagList.at("Projectile") },
			{ "InteractableItem", ignoreTagList.at("InteractableItem") },
			{ "InteractableObject", ignoreTagList.at("InteractableObject") },
			{ "Item", ignoreTagList.at("Item") },
			});

		// Get object name
		std::string objectName = currentObject.GetName();
		Vector2 spawnLocation = currentObject.GetPrefabSpawnLocation();
		if (currentObject.HasComponent("Transform"))
			spawnLocation = currentObject.GetTransform()->GetPosition();

		// Create Game Object Json data object
		json gameObjectJson = json::object({
			{ "_isPrefab", currentObject.IsPrefab() },
			{ "prefabName", currentObject.GetPrefabName() },
			{ "spawnLocationX", spawnLocation.x },
			{ "spawnLocationY", spawnLocation.y },
			{ "name", objectName },
			{ "id", currentObject.GetID() },
			{ "_isActive", currentObject.IsActive() },
			{ "parent", currentObject.GetParentID() },
			{ "children", childrenArray },
			{ "components", componentsArray },
			{ "tags", tagsObjectJson },
			{ "ignoreTags", ignoreTagsObjectJson },
			});

		return gameObjectJson;
	}
	bool JsonContains(json obj, std::string checkFor, std::string loadedName)
	{
		bool contains = false;
		if (obj.contains(checkFor))
			contains = true;
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return contains;
	}
	float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName)
	{
		float value = 0;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
	{
		int value = 0;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	long CheckJsonLong(json obj, std::string checkFor, std::string loadedName)
	{
		long value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName)
	{
		bool value = false;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName)
	{
		std::string value = "Failed to load.";
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	GameObject CreateObjectFromJson(json objectJson)
	{
		GameObject loadedObject;
		std::string objectName = CheckJsonString(objectJson, "name", "Name");
		bool _isActive = CheckJsonBool(objectJson, "_isActive", objectName);
		bool _isPrefab = CheckJsonBool(objectJson, "_isPrefab", objectName);
		std::string prefabName = CheckJsonString(objectJson, "prefabName", objectName);
		Vector2 spawnLocation = Vector2(0, 0);
		spawnLocation.x = CheckJsonFloat(objectJson, "spawnLocationX", objectName); // SetOrigin() is taken care of by Instantiate() using parentID
		spawnLocation.y = CheckJsonFloat(objectJson, "spawnLocationY", objectName);
		long loadedID = CheckJsonLong(objectJson, "id", objectName);
		long loadedParentID = CheckJsonLong(objectJson, "parent", objectName);
		std::vector<long> loadedChildrenIDs = std::vector<long>();
		TagList tags = TagList();

		if (JsonContains(objectJson, "children", objectName))
		{
			for (int c = 0; c < objectJson["children"].size(); c++)
			{
				loadedChildrenIDs.push_back(objectJson["children"][c]);
			}
		}

		// Instantiate Prefab (if it is one) using PrefabManager (keeps objects up-to-date with changes made to Prefab json)
		if (_isPrefab)
		{
			loadedObject = Instantiate(prefabName, spawnLocation, loadedParentID, loadedID);
		}
		else // If object does not originate from a Prefab, load it normally
		{
			loadedObject = GameObject(loadedParentID, loadedID);

			// TagList
			bool b_updateColliderPairs = false;
			if (JsonContains(objectJson, "tags", objectName))
			{
				json tagsObj = objectJson["tags"];
				tags.SetTag("Player", CheckJsonBool(tagsObj, "Player", objectName), b_updateColliderPairs);
				tags.SetTag("Enemy", CheckJsonBool(tagsObj, "Enemy", objectName), b_updateColliderPairs);
				tags.SetTag("Npc", CheckJsonBool(tagsObj, "Npc", objectName), b_updateColliderPairs);
				tags.SetTag("Terrain", CheckJsonBool(tagsObj, "Terrain", objectName), b_updateColliderPairs);
				tags.SetTag("PlayerTrigger", CheckJsonBool(tagsObj, "PlayerTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("EnemyTrigger", CheckJsonBool(tagsObj, "EnemyTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("NpcTrigger", CheckJsonBool(tagsObj, "NpcTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("EnvironmentalTrigger", CheckJsonBool(tagsObj, "EnvironmentalTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("TerrainTrigger", CheckJsonBool(tagsObj, "TerrainTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName), b_updateColliderPairs);
				tags.SetTag("PlayerDamage", CheckJsonBool(tagsObj, "PlayerDamage", objectName), b_updateColliderPairs);
				tags.SetTag("EnemyDamage", CheckJsonBool(tagsObj, "EnemyDamage", objectName), b_updateColliderPairs);
				tags.SetTag("EnvironmentalDamage", CheckJsonBool(tagsObj, "EnvironmentalDamage", objectName), b_updateColliderPairs);
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName), b_updateColliderPairs);
				tags.SetTag("InteractableItem", CheckJsonBool(tagsObj, "InteractableItem", objectName), b_updateColliderPairs);
				tags.SetTag("InteractableObject", CheckJsonBool(tagsObj, "InteractableObject", objectName), b_updateColliderPairs);
				tags.SetTag("Item", CheckJsonBool(tagsObj, "Item", objectName), b_updateColliderPairs);
			}
			if (JsonContains(objectJson, "ignoreTags", objectName))
			{
				json ignoreTags = objectJson["ignoreTags"];
				tags.SetIgnore("Player", CheckJsonBool(ignoreTags, "Player", objectName), b_updateColliderPairs);
				tags.SetIgnore("Enemy", CheckJsonBool(ignoreTags, "Enemy", objectName), b_updateColliderPairs);
				tags.SetIgnore("Npc", CheckJsonBool(ignoreTags, "Npc", objectName), b_updateColliderPairs);
				tags.SetIgnore("Terrain", CheckJsonBool(ignoreTags, "Terrain", objectName), b_updateColliderPairs);
				tags.SetIgnore("PlayerTrigger", CheckJsonBool(ignoreTags, "PlayerTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnemyTrigger", CheckJsonBool(ignoreTags, "EnemyTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("NpcTrigger", CheckJsonBool(ignoreTags, "NpcTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnvironmentalTrigger", CheckJsonBool(ignoreTags, "EnvironmentalTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("TerrainTrigger", CheckJsonBool(ignoreTags, "TerrainTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName), b_updateColliderPairs);
				tags.SetIgnore("PlayerDamage", CheckJsonBool(ignoreTags, "PlayerDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnemyDamage", CheckJsonBool(ignoreTags, "EnemyDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnvironmentalDamage", CheckJsonBool(ignoreTags, "EnvironmentalDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName), b_updateColliderPairs);
				tags.SetIgnore("InteractableItem", CheckJsonBool(ignoreTags, "InteractableItem", objectName), b_updateColliderPairs);
				tags.SetIgnore("InteractableObject", CheckJsonBool(ignoreTags, "InteractableObject", objectName), b_updateColliderPairs);
				tags.SetIgnore("Item", CheckJsonBool(ignoreTags, "Item", objectName), b_updateColliderPairs);
			}
			loadedObject.SetTagList(tags);

			float objectRotation = 0;

			// Loop through the components in this GameObjects json
			for (int j = 0; j < objectJson["components"].size(); j++)
			{
				json componentJson = objectJson["components"][j];
				std::string type = CheckJsonString(componentJson, "type", objectName);
				long id = CheckJsonLong(componentJson, "id", objectName);
				bool _isCollapsed = CheckJsonBool(componentJson, "_isCollapsed", objectName);
				bool _isActive = CheckJsonBool(componentJson, "_isActive", objectName);

				//Add each loaded component to the newly created GameObject
				if (type == "Transform")
				{
					Transform* newTransform = loadedObject.AddTransformComponent(id, _isActive, _isCollapsed);
					float rotation = CheckJsonFloat(componentJson, "rotation", objectName);
					newTransform->SetOrigin(Vector2(CheckJsonFloat(componentJson, "xOrigin", objectName), CheckJsonFloat(componentJson, "yOrigin", objectName)));
					newTransform->SetInitialPosition(Vector2(CheckJsonFloat(componentJson, "xPos", objectName), CheckJsonFloat(componentJson, "yPos", objectName)));
					newTransform->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					newTransform->SetRotation(rotation);
					objectRotation = rotation;
				}
				else if (type == "Sprite")
				{
					Sprite* newSprite = loadedObject.AddSpriteComponent(id, _isActive, _isCollapsed);
					std::string pivotPoint = "Center";
					if (CheckJsonString(componentJson, "pivotPoint", objectName) != "")
						pivotPoint = CheckJsonString(componentJson, "pivotPoint", objectName);
					newSprite->SetPivotPoint(pivotPoint);
					newSprite->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					newSprite->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
					newSprite->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newSprite->SetTintColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
					newSprite->SetTexture(CheckJsonString(componentJson, "path", objectName));
				}
				else if (type == "Camera")
				{
					Camera* newCamera = loadedObject.AddCameraComponent(id, _isActive, _isCollapsed);
					bool _isPrimaryCamera = CheckJsonBool(componentJson, "_isPrimaryCamera", objectName);
					newCamera->SetDimensions(CheckJsonFloat(componentJson, "width", objectName), CheckJsonFloat(componentJson, "height", objectName));
					newCamera->SetPrimaryCamera(_isPrimaryCamera);
					newCamera->SetZoom(CheckJsonFloat(componentJson, "zoom", objectName));
					newCamera->SetFrustrumColor(Vector4(
						CheckJsonFloat(componentJson, "f_red", objectName),
						CheckJsonFloat(componentJson, "f_green", objectName),
						CheckJsonFloat(componentJson, "f_blue", objectName),
						CheckJsonFloat(componentJson, "f_alpha", objectName)
					));
					newCamera->SetShouldFollow(CheckJsonBool(componentJson, "_follow", objectName));
					newCamera->SetFollowSmoothing(CheckJsonFloat(componentJson, "followSmoothing", objectName));
					newCamera->SetFollowing(CheckJsonLong(componentJson, "following", objectName));
				}
				else if (type == "Script")
				{
					ScriptComponent* newScript = loadedObject.AddScriptComponent(id, _isActive, _isCollapsed);
					newScript->SetAttachedScript(CheckJsonString(componentJson, "attachedScript", objectName));
				}
				else if (type == "Button")
				{
					Button* newButton = loadedObject.AddButtonComponent(id, _isActive, _isCollapsed);
					newButton->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newButton->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newButton->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
				}
				else if (type == "Canvas")
				{
					Canvas* newCanvas = loadedObject.AddCanvasComponent(id, _isActive, _isCollapsed);
					newCanvas->SetDimensions(CheckJsonFloat(componentJson, "canvasWidth", objectName), CheckJsonFloat(componentJson, "canvasHeight", objectName));
					newCanvas->SetLayerNumber(CheckJsonInt(componentJson, "layerNumber", objectName));
					newCanvas->SetBlocksLayers(CheckJsonBool(componentJson, "_blocksLayers", objectName));
				}
				else if (type == "Animation")
				{
					Animation* newAnimation = loadedObject.AddAnimationComponent(id, _isActive, _isCollapsed);
					newAnimation->SetAnimationPath(CheckJsonString(componentJson, "path", objectName));
				}
				else if (type == "Audio")
				{
					Audio* newAudio = loadedObject.AddAudioComponent(id, _isActive, _isCollapsed);
					newAudio->SetIsMusic(CheckJsonBool(componentJson, "_isMusic", objectName));
					newAudio->SetPath(CheckJsonString(componentJson, "path", objectName));
				}
				else if (type == "Text")
				{
					Text* newText = loadedObject.AddTextComponent(id, _isActive, _isCollapsed);
					newText->SetFontPath(CheckJsonString(componentJson, "fontPath", objectName));
					newText->SetFontSize(CheckJsonInt(componentJson, "fontSize", objectName));
					newText->SetColor(Vector4(
						CheckJsonFloat(componentJson, "f_red", objectName),
						CheckJsonFloat(componentJson, "f_green", objectName),
						CheckJsonFloat(componentJson, "f_blue", objectName),
						CheckJsonFloat(componentJson, "f_alpha", objectName)
					));
					newText->SetText(CheckJsonString(componentJson, "text", objectName));
					newText->SetOffset(Vector2(CheckJsonFloat(componentJson, "offsetX", objectName), CheckJsonFloat(componentJson, "offsetY", objectName)));
					newText->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newText->LoadText();
				}
				else if (type == "CharacterController")
				{
					CharacterController* newCharacterController = loadedObject.AddCharacterControllerComponent(id, _isActive, _isCollapsed);
					newCharacterController->SetMaxAcceleration(CheckJsonFloat(componentJson, "maxAcceleration", objectName));
					newCharacterController->SetMaxSpeed(CheckJsonFloat(componentJson, "maxSpeed", objectName));
					newCharacterController->SetAirControl(CheckJsonFloat(componentJson, "airControl", objectName));
				}
				else if (type == "BoxCollider")
				{
					BoxCollider* newBoxCollider = loadedObject.AddBoxColliderComponent(id, _isActive, _isCollapsed);
					newBoxCollider->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newBoxCollider->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newBoxCollider->SetIsContinuous(CheckJsonBool(componentJson, "_isContinuous", objectName));
					newBoxCollider->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newBoxCollider->SetIsSolid(CheckJsonBool(componentJson, "_isSolid", objectName));
					newBoxCollider->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
					newBoxCollider->SetRotation(objectRotation);
					newBoxCollider->SetShowActiveRadius(CheckJsonBool(componentJson, "_showActiveRadius", objectName));
					newBoxCollider->SetIsComposite(CheckJsonBool(componentJson, "_isComposite", objectName));
				}
				else if (type == "CircleCollider")
				{
					CircleCollider* newCircleCollider = loadedObject.AddCircleColliderComponent(id, _isActive, _isCollapsed);
					newCircleCollider->SetActiveRadiusGrid(CheckJsonFloat(componentJson, "activeRadius", objectName));
					newCircleCollider->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newCircleCollider->SetIsContinuous(CheckJsonBool(componentJson, "_isContinuous", objectName));
					newCircleCollider->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newCircleCollider->SetIsSolid(CheckJsonBool(componentJson, "_isSolid", objectName));
					newCircleCollider->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
					newCircleCollider->SetIsComposite(CheckJsonBool(componentJson, "_isComposite", objectName));
				}
				else if (type == "RigidBody")
				{
					RigidBody* newRigidBody = loadedObject.AddRigidBodyComponent(id, _isActive, _isCollapsed);
					newRigidBody->SetMass(CheckJsonFloat(componentJson, "mass", objectName));
					newRigidBody->SetAngularDrag(CheckJsonFloat(componentJson, "angularDrag", objectName));
					newRigidBody->SetGravity(CheckJsonFloat(componentJson, "gravity", objectName));
					newRigidBody->SetFriction(CheckJsonFloat(componentJson, "friction", objectName));
					newRigidBody->SetWindResistance(CheckJsonFloat(componentJson, "windResistance", objectName));
					newRigidBody->SetEquilibriumForce(CheckJsonFloat(componentJson, "equilibriumForce", objectName));
					newRigidBody->SetTerminalVelocity(CheckJsonFloat(componentJson, "terminalVelocity", objectName));
					newRigidBody->SetIsKinematic(CheckJsonBool(componentJson, "_isKinematic", objectName));
					newRigidBody->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
				}
			}
		}

		loadedObject.SetName(objectName);
		loadedObject.SetActive(_isActive);

		// Set children after because they may not be created yet and SetActive() calls child objects
		for (int c = 0; c < objectJson["children"].size(); c++)
		{
			loadedObject.AddChild(loadedChildrenIDs[c]);
		}

		return loadedObject;
	}


	// Rendering
	void BeginImGuiRender()
	{
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Create dockable background space for all viewports
		ImGui::DockSpaceOverViewport();
	}
	
	void EndImGuiRender()
	{
		// Rendering
		Vector4 clear_color = Vector4(1.00f, 1.00f, 1.00f, 1.00f);
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		SDL_RenderSetScale(Window::W_Renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(Window::W_Renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(Window::W_Renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

		// TO DO 
		// When vsync is off, find a way to only call renderpresent once per refresh depending on the rate of the screen
		//Uint32 renderPresentStart = FlatEngine::GetEngineTime(); // Profiler
		SDL_RenderPresent(Window::W_Renderer);
		//FlatGui::AddProcessData("Render Present", (float)FlatEngine::GetEngineTime() - renderPresentStart); // Profiler
	}


	void SetNextViewportToFillWindow()
	{
		ImGuiIO io = ImGui::GetIO();
		Vector2 f_displaySize = io.DisplaySize;
		ImGui::SetNextWindowSize(f_displaySize);
		ImGui::SetNextWindowPos({ 0,0 });
	}

	// Animations
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


		for (std::shared_ptr<Animation::S_Event> eventProp : propertiesObject->eventProperties)
		{
			// Declare components array json object for components
			json eventPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "functionName", eventProp->functionName },
				{ "time", eventProp->time },
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			eventPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json eventProperty = json::object({
				{ "Property", "Event" },
				{ "Frames", eventPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(eventProperty);
		}
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
				{ "yOffset", spriteProp->yOffset },
				{ "tintColorX", spriteProp->tintColor.x },
				{ "tintColorY", spriteProp->tintColor.y },
				{ "tintColorZ", spriteProp->tintColor.z },
				{ "tintColorW", spriteProp->tintColor.w },
				{ "_instantTintChange", spriteProp->_instantTintChange },
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
		std::shared_ptr<Animation::S_Event> eventProperties;
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
						// Event property
						if (currentObjectJson["Property"] == "Event")
						{
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									std::shared_ptr<Animation::S_Event> eventFrames = std::make_shared<Animation::S_Event>();
									eventFrames->name = "Event";
									if (currentObjectJson["Frames"][f]["functionName"] != "")
										eventFrames->functionName = currentObjectJson["Frames"][f]["functionName"];
									eventFrames->time = currentObjectJson["Frames"][f]["time"];

									// Save the data to the animationProperties struct
									animationProperties->eventProperties.push_back(eventFrames);
								}
							}
						}
						// Transform property
						else if (currentObjectJson["Property"] == "Transform")
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
						// Sprite property
						else if (currentObjectJson["Property"] == "Sprite")
						{
							if (currentObjectJson.contains("Frames"))
							{
								for (int f = 0; f < currentObjectJson["Frames"].size(); f++)
								{
									Vector4 tintColor = Vector4(1, 1, 1, 1);
									std::shared_ptr<Animation::S_Sprite> spriteFrames = std::make_shared<Animation::S_Sprite>();
									spriteFrames->name = "Sprite";
									if (currentObjectJson["Frames"][f]["interpType"] == "Lerp")
										spriteFrames->interpType = Animation::InterpType::Lerp;
									spriteFrames->speed = currentObjectJson["Frames"][f]["speed"];
									spriteFrames->time = currentObjectJson["Frames"][f]["time"];
									spriteFrames->xOffset = currentObjectJson["Frames"][f]["xOffset"];
									spriteFrames->yOffset = currentObjectJson["Frames"][f]["yOffset"];
									spriteFrames->path = currentObjectJson["Frames"][f]["path"];
									if (currentObjectJson["Frames"][f].contains("tintColorX"))
										tintColor.x = currentObjectJson["Frames"][f]["tintColorX"];
									if (currentObjectJson["Frames"][f].contains("tintColorY"))
										tintColor.y = currentObjectJson["Frames"][f]["tintColorY"];
									if (currentObjectJson["Frames"][f].contains("tintColorZ"))
										tintColor.z = currentObjectJson["Frames"][f]["tintColorZ"];
									if (currentObjectJson["Frames"][f].contains("tintColorW"))
										tintColor.w = currentObjectJson["Frames"][f]["tintColorW"];
									spriteFrames->tintColor = tintColor;
									if (currentObjectJson["Frames"][f].contains("_instantTintChange"))
										spriteFrames->_instantTintChange = currentObjectJson["Frames"][f]["_instantTintChange"];

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


	// Prefabs
	void CreatePrefab(std::string path, GameObject &gameObject)
	{
		prefabManager->CreatePrefab(path, gameObject);
	}

	void InitializePrefabs()
	{
		prefabManager->InitializePrefabs();
	}

	GameObject Instantiate(std::string prefabName, Vector2 position, long parentID, long ID)
	{
		return prefabManager->Instantiate(prefabName, position, parentID, ID);
	}
	
	std::map<std::string, std::vector<GameObject>> GetPrefabs()
	{
		return prefabManager->GetPrefabs();
	}


	// Logging
	void LogString(std::string line)
	{
		F_Logger.LogString(line);
	}

	void LogSeparator()
	{
		F_Logger.LogSeparator();
	}

	void LogFloat(float var, std::string line)
	{
		F_Logger.LogFloat(var, line);
	}

	void LogInt(int var, std::string line)
	{
		F_Logger.LogInt(var, line);
	}

	void LogVector2(Vector2 vector, std::string line)
	{
		F_Logger.LogVector2(vector, line);
	}

	void DrawRectangle(Vector2 startingPoint, Vector2 endPoint, Vector2 canvas_p0, Vector2 canvas_sz, Vector4 color, float thickness, ImDrawList* drawList)
	{
		if (startingPoint.x < canvas_p0.x)
			startingPoint.x = canvas_p0.x;
		if (endPoint.x > canvas_p0.x + canvas_sz.x)
			endPoint.x = canvas_p0.x + canvas_sz.x;
		if (startingPoint.y < canvas_p0.y)
			startingPoint.y = canvas_p0.y;
		if (endPoint.y > canvas_p0.y + canvas_sz.y)
			endPoint.y = canvas_p0.y + canvas_sz.y;

		F_Logger.DrawRectangle(startingPoint, endPoint, color, thickness, drawList);
	}

	void DebugRectangle(Vector2 startingPoint, Vector2 endPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		F_Logger.DrawRectangle(startingPoint, endPoint, color, thickness, drawList);
	}

	void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		F_Logger.DrawLine(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawRectangleFromLines(Vector2* corners, Vector4 color, float thickness, ImDrawList* drawList)
	{
		DrawLine(corners[0], corners[1], color, thickness, drawList);
		DrawLine(corners[1], corners[2], color, thickness, drawList);
		DrawLine(corners[2], corners[3], color, thickness, drawList);
		DrawLine(corners[3], corners[0], color, thickness, drawList);
	}

	void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness, int segments)
	{
		drawList->AddCircle(center, radius, ImGui::GetColorU32(color), segments, thickness);
	}

	void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList)
	{
		F_Logger.DrawPoint(point, color, drawList);
	}


	// Game Loop
	void StartGameLoop()
	{
		F_Application->StartGameLoop();
	}

	void GameLoopUpdate()
	{
		F_Application->UpdateGameLoop();
	}

	void PauseGameLoop()
	{
		F_Application->PauseGameLoop();
	}

	void PauseGame()
	{
		F_Application->PauseGame();
	}

	void StopGameLoop()
	{
		F_Application->StopGameLoop();
	}

	float GetEllapsedGameTimeInSec()
	{
		return F_Application->GetGameLoop()->TimeEllapsedInSec();
	}

	long GetEllapsedGameTimeInMs()
	{
		return F_Application->GetGameLoop()->TimeEllapsedInMs();
	}

	bool GameLoopStarted()
	{
		return F_Application->GetGameLoop()->IsStarted();
	}

	bool GameLoopPaused()
	{
		return F_Application->GetGameLoop()->IsPaused();
	}

	float GetAverageFps()
	{
		return F_Application->GetGameLoop()->GetAverageFps();
	}

	float GetDeltaTime()
	{
		return F_Application->GetGameLoop()->GetDeltaTime();
	}


	// Helper
	// 
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
		//  Removes absolute path from the beginning of the selected filepath
		const size_t currentDirIndex = sFilePath.find("FlatEngine-Editor");
		sSelectedFile = sFilePath.substr(slash);
		//  SUCCESS, CLEAN UP
		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return sFilePath.substr(currentDirIndex + 18);
	}

	std::string GetFilenameFromPath(std::string path, bool _keepExtension)
	{
		std::string finalName;

		//  FORMAT STRING
		const size_t slash = path.find_last_of("/\\");
		std::string wholeFilename = path.substr(slash + 1);
		const size_t dot = wholeFilename.find_last_of(".");
		std::string extension = wholeFilename.substr(dot);

		if (!_keepExtension)
			finalName = wholeFilename.substr(0, wholeFilename.size() - extension.size());
		else
			finalName = wholeFilename;

		return finalName;
	}

	std::string GetCurrentDir()
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		std::wstring ws = std::wstring(buffer).substr(0, pos);
		std::string dir(ws.begin(), ws.end());
		return dir;
	}

	//Vector4 objectA(top, right, bottom, left), Vector4 objectB(top, right, bottom, left)
	bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB)
	{
		float A_TopEdge = ObjectA.z;
		float A_RightEdge = ObjectA.y;
		float A_BottomEdge = ObjectA.x;
		float A_LeftEdge = ObjectA.w;

		float B_TopEdge = ObjectB.x;
		float B_RightEdge = ObjectB.y;
		float B_BottomEdge = ObjectB.z;
		float B_LeftEdge = ObjectB.w;

		return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
	}

	Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease)
	{
		Vector2 difference = Vector2(endPos.x - startPos.x, endPos.y - startPos.y);
		Vector2 easedDiff = Vector2(difference.x * ease, difference.y * ease);
		return Vector2(startPos.x + easedDiff.x, startPos.y + easedDiff.y);
	}
}