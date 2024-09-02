#include "FlatEngine.h"
#include "UIManager.h"
#include "MappingContext.h"
#include "PrefabManager.h"
#include "Logger.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Window.h"
#include "ECSManager.h"
#include "Project.h"
#include "AssetManager.h"
#include "MappingContext.h"
#include "TileSet.h"

#include "GameLoop.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "BoxCollider.h"
#include "Script.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "Text.h"
#include "Texture.h"
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
	std::string F_RuntimeDirectoriesLuaFilepath = "../engine/scripts/DebugDirectories.lua";
	std::string F_EditorDirectoriesLuaFilepath = "../engine/scripts/RuntimeDirectories.lua";;
	std::string F_DebugDirectoriesLuaFilepath = "../engine/scripts/EditorDirectories.lua";;
	std::shared_ptr<Application> F_Application = std::make_shared<Application>();
	AssetManager F_AssetManager = AssetManager();
	std::vector<std::string> F_selectedFiles = std::vector<std::string>();

	F_CURSOR_MODE F_CursorMode = F_CURSOR_MODE::TRANSLATE;
	bool F_b_sceneViewFocused = false;


	bool _isDebugMode = true;
	bool _closeProgram = false;

	std::vector<SDL_Joystick*> gamepads = std::vector<SDL_Joystick*>();
	int JOYSTICK_DEAD_ZONE = 4000;

	// For rendering sprites
	int F_maxSpriteLayers = 55;

	// sprite size * 0.1f brings sprite down from 1 sprite pixel per 1 grid block to 10 sprite pixels per 1 grid block
	// then to make an 8x8 sprite fit properly into a 10px by 10px grid block
	// take the ratio of 10px / 8px and multiply it by our now scaled down sprite size to get:  0.1 * (10 / 8) = 0.125 = the scale multiplier
	// Now our 8x8, 16x16, 32x32, etc, pixel art fits nicely inside the grid space blocks
	float F_pixelsPerGridSpace = 8.0f;
	float F_spriteScaleMultiplier = 0.1f * (10.0f / F_pixelsPerGridSpace);

	GameObject* F_PlayerObject = nullptr;
	Project F_LoadedProject = Project();

	// Managers
	Logger F_Logger = Logger();
	UIManager F_UIManager = UIManager();
	SceneManager F_SceneManager = SceneManager();	
	Sound F_SoundController = Sound();
	std::vector<MappingContext> F_MappingContexts = std::vector<MappingContext>();
	std::string F_selectedMappingContextName = "";
	TTF_Font* F_fontCinzel;
	std::shared_ptr<PrefabManager> prefabManager = std::make_shared<PrefabManager>();
	std::vector<TileSet> F_TileSets = std::vector<TileSet>();
	std::string F_selectedTileSetToEdit = "";
	std::pair<std::string, int> F_tileSetAndIndexOnBrush = std::pair<std::string, int>();

	// Drag/Drop target IDs
	std::string F_fileExplorerTarget = "DND_FILE_PATH_OBJECT";
	std::string F_hierarchyTarget = "DND_HIERARCHY_OBJECT";

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

	// Get directory path using name given in Directories.lua
	std::string GetDir(std::string dirName)
	{
		return F_AssetManager.GetDir(dirName);
	}

	// Get file path using name given in Directories.lua
	std::string GetFilePath(std::string fileName)
	{
		return F_AssetManager.GetFilePath(fileName);
	}
	
	// Get entire std::shared_ptr<Texture> object using name given in Textures.lua file
	std::shared_ptr<Texture> GetTextureObject(std::string textureName)
	{
		return F_AssetManager.GetTextureObject(textureName);
	}

	// Get SDL_Texture* using name given in Textures.lua file
	SDL_Texture* GetTexture(std::string textureName)
	{
		return F_AssetManager.GetTexture(textureName);
	}

	// Get color using name given in Colors.lua file
	Vector4 GetColor(std::string colorName)
	{
		return F_AssetManager.GetColor(colorName);
	}

	// Get color using name given in Colors.lua file converted to Uint32 format
	Uint32 GetColor32(std::string colorName)
	{
		return F_AssetManager.GetColor32(colorName);
	}

	Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation, ImU32 addColor, Vector2 uvStart, Vector2 uvEnd)
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
			draw_list->AddImage((void*)texture, renderStart, renderEnd, uvStart, uvEnd, addColor);
		}

		return renderStart;
	}


	bool Init(int windowWidth, int windowHeight, DirectoryType dirType)
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
			LogString("SDL initialized... - Video - Audio - Joystick -");

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
				LogString("Window initialized...");

				//Initialize SDL_image for png loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					LogString("SDL_image initialized...");
					//Initialize SDL_ttf for text rendering
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
					else
					{
						LogString("TTF_Fonts initialized...");
						//Initialize SDL_mixer
						if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
						{
							printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
							success = false;
						}
						else
						{
							Mix_AllocateChannels(100);							   // Sets number of individual audios that can play at once
							LogString("SDL_mixer initialized...");

							InitLua();
							LogString("Lua initialized...");

							F_AssetManager.CollectDirectories(dirType);		       // Collect important directories and file paths from xxxDirectories.lua (depending on the build type, uses paths populated just above)
							F_AssetManager.CollectColors();						   // Collect global colors from Colors.lua
							F_AssetManager.CollectTextures();				       // Collect and create Texture icons from Textures.lua
							SetupImGui();										   // Setup ImGui for use in the prompt for Directories.lua location
							SetImGuiColors();									   // Use the collected colors to style ImGui elements
							RetrieveLuaScriptPaths();
							InitializeTileSets();

							LogString("Ready...");
							LogSeparator();
							LogString("Begin Logging...");
							LogSeparator();
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
		SetImGuiColors();  // Colors will not be loaded yet, but they will obtain the default color given by FL::GetColor();
	}

	void SetImGuiColors()
	{
		// Round about way of editing the active titlebgactive color since pushstylecolor doesn't seem to work for it.
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName(i);

			if (name == "TitleBgActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("titleBgActive");
			}
			if (name == "TabUnfocusedActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("tabUnfocusedActive");
			}
			if (name == "TabActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("tabActive");
			}
			if (name == "ResizeGripHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("resizeGripHovered");
			}
			if (name == "ResizeGripActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("resizeGripActive");
			}
			if (name == "DockingPreview")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("dockingPreview");
			}
			if (name == "DockingEmptyBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("dockingPreviewEmpty");
			}
		}
	}

	void RestartImGui()
	{
		QuitImGui();
		SetupImGui();
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


	Uint32 GetEngineTime()
	{
		return (int)SDL_GetTicks();
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

	void LoadGameProject(std::string path, json &projectJson)
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
			projectJson = json::parse(fileContent);

			if (projectJson["Project Properties"][0] != "NULL")
			{
				// Loop through the saved Properties in the JSON file
				for (int i = 0; i < projectJson["Project Properties"].size(); i++)
				{
					// Get data from the loaded object
					json currentObjectJson = projectJson["Project Properties"][i];

					// Open items
					if (currentObjectJson.contains("path"))
						newProject.SetPath(currentObjectJson["path"]);
					if (currentObjectJson.contains("loadedScenePath"))
						newProject.SetLoadedScenePath(currentObjectJson["loadedScenePath"]);
					if (currentObjectJson.contains("buildPath"))
						newProject.SetBuildPath(currentObjectJson["buildPath"]);
					if (currentObjectJson.contains("loadedAnimationPath"))
						newProject.SetLoadedPreviewAnimationPath(currentObjectJson["loadedAnimationPath"]);
					if (currentObjectJson.contains("focusedGameObjectID"))
						newProject.SetFocusedGameObjectID(currentObjectJson["focusedGameObjectID"]);
					if (currentObjectJson.contains("sceneToLoadAtRuntime"))
						newProject.SetRuntimeScene(currentObjectJson["sceneToLoadAtRuntime"]);

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

					if (currentObjectJson.contains("_autoSave"))
						newProject.SetAutoSave(currentObjectJson["_autoSave"]);
					if (currentObjectJson.contains("resolutionWidth") && currentObjectJson.contains("resolutionHeight"))
						newProject.SetResolution(Vector2(currentObjectJson["resolutionWidth"], currentObjectJson["resolutionHeight"]));
					if (currentObjectJson.contains("_fullscreen"))
						newProject.SetFullscreen(currentObjectJson["_fullscreen"]);
					if (currentObjectJson.contains("_vsyncEnabled"))
						newProject.SetVsyncEnabled(currentObjectJson["_vsyncEnabled"]);
				}
			}
		}

		if (newProject.GetLoadedScenePath() != "")
			FL::LoadScene(newProject.GetLoadedScenePath());
		else
			FL::CreateNewScene();

		// Set loaded project
		FL::SetLoadedProject(newProject);
	}

	void BuildProject()
	{
		if (F_LoadedProject.GetBuildPath() != "")
		{
			try
			{
				std::filesystem::copy("../FlatEngine-Core", F_LoadedProject.GetBuildPath() + "\\Core", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogString("ERROR : Failed to copy FlatEngine-Core : ");
				LogString(e.what());
			}
			try
			{
				std::filesystem::copy("../FlatEngine-Runtime", F_LoadedProject.GetBuildPath() + "\\Runtime", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogString("ERROR : Failed to copy FlatEngine-Runtime : ");
				LogString(e.what());
			}
			try
			{
				std::filesystem::copy("../assets", F_LoadedProject.GetBuildPath() + "\\assets", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogString("ERROR : Failed to copy assets : ");
				LogString(e.what());
			}
			try
			{
				std::filesystem::copy("../engine", F_LoadedProject.GetBuildPath() + "\\engine", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogString("ERROR : Failed to copy engine dependencies: ");
				LogString(e.what());
			}
			try
			{
				std::filesystem::copy("../intermediates", F_LoadedProject.GetBuildPath() + "\\intermediates", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogString("ERROR : Failed to copy intermediates: ");
				LogString(e.what());
			}
		}
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

	void CreateNewSceneFile(std::string filename, std::string path)
	{
		Scene* newScene = CreateNewScene();
		newScene->SetName(filename);
		std::string filePath = "";

		if (path == "")
			filePath = GetDir("scenes") + "/" + filename + ".scn";
		else
			filePath = path + "/" + filename + ".scn";

		newScene->SetPath(filePath);

		SaveScene(newScene, filePath);
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


	void CreateNewMappingContextFile(std::string fileName, std::string path)
	{
		std::string filePath = "";		
		MappingContext newContext = MappingContext();

		if (path == "")
			filePath = GetDir("mappingContexts") + "/" + fileName + ".mpc";
		else
			filePath = path + "/" + fileName + ".mpc";

		newContext.SetPath(filePath);
		newContext.SetName(fileName);
		SaveMappingContext(filePath, newContext);
		FL::InitializeMappingContexts();
	}

	GameObject* CreateAssetUsingFilePath(std::string filePath, Vector2 position)
	{
		std::string extension = std::filesystem::path(filePath).extension().string();

		// Scene file
		if (extension == ".png")
		{
			GameObject* newObject = CreateGameObject();
			newObject->SetName(GetFilenameFromPath(filePath) + std::to_string(newObject->GetID()));
			newObject->GetTransform()->SetPosition(position);
			newObject->AddSprite()->SetTexture(filePath);
			return newObject;
		}
		else return nullptr;
		//else if (extension == ".scn")
		//{

		//}
		//// Mapping Context file
		//else if (extension == ".mpc")
		//{

		//}
		//// Animation file
		//else if (extension == ".anm")
		//{

		//}
		//// Lua file
		//else if (extension == ".scp")
		//{

		//}
		//// Project file
		//else if (extension == ".prj")
		//{
		//
		//}
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

		std::vector<std::string> mappingContextFiles = std::vector<std::string>();
		mappingContextFiles = FindAllFilesWithExtension(GetDir("projectDir"), ".mpc");

		for (std::string path : mappingContextFiles)
		{
			// Create a new context to save the loaded keybindings to
			MappingContext newContext = MappingContext();

			json contextData = LoadFileData(path);
			if (contextData != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto mappings = contextData["Mapping Context"][0];
				
				newContext.SetName(mappings["name"]);
				newContext.SetPath(path);
				
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

	// Events
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
			for (MappingContext &context : F_MappingContexts)
				context.UnFireEvent(keybind);
		firedKeys.clear();


		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			if (event.type == SDL_WINDOWEVENT)
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					if (event.window.windowID == SDL_GetWindowID(Window::W_Window))
						quit = true;
						break;

				case SDL_WINDOWEVENT_RESIZED:
					F_Application->WindowResized();
					break;
				
				case SDL_WINDOWEVENT_MAXIMIZED:
					F_Application->WindowResized();
					break;

				default:
					break;
				}
			}

			HandleEngineEvents(event);

			for (MappingContext &context : F_MappingContexts)
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

	void HandleEngineEvents(SDL_Event event)
	{
		// Keyboard Keys Down
		if (event.type == SDL_KEYDOWN)
		{
			// Scene View inputs
			if (F_b_sceneViewFocused)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_t:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
					break;

				case SDLK_w:
					FL::F_CursorMode = FL::F_CURSOR_MODE::SCALE;
					break;

				case SDLK_r:
					FL::F_CursorMode = FL::F_CURSOR_MODE::ROTATE;
					break;

				case SDLK_b:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
					break;

				case SDLK_e:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_ERASE;
					break;

				case SDLK_c:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
					break;

				case SDLK_m:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MOVE;
					break;

				case SDLK_s:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MULTISELECT;
					break;

				case SDLK_SPACE:
					FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
					
					//ImGuiIO& inputOutput = ImGui::GetIO();
					//Vector2 currentPos = ImGui::GetCursorScreenPos();
					//bool _weightedScroll = true;
					//Vector2 endPos = Vector2(startPos.x + size.x, startPos.y + size.y);


					//// Border
					//if (b_filled)
					//	ImGui::GetWindowDrawList()->AddRectFilled(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
					//else
					//	ImGui::GetWindowDrawList()->AddRect(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);

					////ImGui::SetNextItemAllowOverlap();
					//FL::RenderInvisibleButton(buttonID.c_str(), startPos, size, b_allowOverlap, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					//const bool is_hovered = ImGui::IsItemHovered(); // Hovered
					//const bool is_active = ImGui::IsItemActive();   // Held
					//const bool is_clicked = ImGui::IsItemClicked();

					//// For panning the scene view
					//const float mouse_threshold_for_pan = 0.0f;
					//if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
					//{
					//	// This does not seem to work properly when resizing the window
					//	// inputOutput.MousePos and MouseDelta give incorrect values after upon dragging the mouse
					//	scrolling.x += inputOutput.MouseDelta.x;
					//	scrolling.y += inputOutput.MouseDelta.y;
					//}
					break;
				}
			}
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
				if (context.GetKeyBinding("SDLK_SPACE") != "" && context.GetKeyBoundEvent("SDLK_SPACE").type == 0)
				{
					context.OnInputEvent("SDLK_SPACE", event);
					firedKeys.push_back("SDLK_SPACE");
				}
				break;

			case SDLK_UP:			
				if (context.GetKeyBinding("SDLK_UP") != "" && context.GetKeyBoundEvent("SDLK_UP").type == 0)
				{
					context.OnInputEvent("SDLK_UP", event);
					firedKeys.push_back("SDLK_UP");
				}
				break;

			case SDLK_DOWN:
				if (context.GetKeyBinding("SDLK_DOWN") != "" && context.GetKeyBoundEvent("SDLK_DOWN").type == 0)
				{
					context.OnInputEvent("SDLK_DOWN", event);
					firedKeys.push_back("SDLK_DOWN");
				}
				break;

			case SDLK_LEFT:
				if (context.GetKeyBinding("SDLK_LEFT") != "" && context.GetKeyBoundEvent("SDLK_LEFT").type == 0)
				{
					context.OnInputEvent("SDLK_LEFT", event);
					firedKeys.push_back("SDLK_LEFT");
				}
				break;

			case SDLK_RIGHT:				
				if (context.GetKeyBinding("SDLK_RIGHT") != "" && context.GetKeyBoundEvent("SDLK_RIGHT").type == 0)
				{
					context.OnInputEvent("SDLK_RIGHT", event);
					firedKeys.push_back("SDLK_RIGHT");
				}
				break;

			case SDLK_a:				
				if (context.GetKeyBinding("SDLK_a") != "" && context.GetKeyBoundEvent("SDLK_a").type == 0)
				{
					context.OnInputEvent("SDLK_a", event);
					firedKeys.push_back("SDLK_a");
				}
				break;

			case SDLK_b:				
				if (context.GetKeyBinding("SDLK_b") != "" && context.GetKeyBoundEvent("SDLK_b").type == 0)
				{
					context.OnInputEvent("SDLK_b", event);
					firedKeys.push_back("SDLK_b");
				}
				break;

			case SDLK_c:				
				if (context.GetKeyBinding("SDLK_c") != "" && context.GetKeyBoundEvent("SDLK_c").type == 0)
				{
					context.OnInputEvent("SDLK_c", event);
					firedKeys.push_back("SDLK_c");
				}
				break;

			case SDLK_d:				
				if (context.GetKeyBinding("SDLK_d") != "" && context.GetKeyBoundEvent("SDLK_d").type == 0)
				{
					context.OnInputEvent("SDLK_d", event);
					firedKeys.push_back("SDLK_d");
				}
				break;

			case SDLK_e:				
				if (context.GetKeyBinding("SDLK_e") != "" && context.GetKeyBoundEvent("SDLK_e").type == 0)
				{
					context.OnInputEvent("SDLK_e", event);
					firedKeys.push_back("SDLK_e");
				}
				break;

			case SDLK_f:				
				if (context.GetKeyBinding("SDLK_f") != "" && context.GetKeyBoundEvent("SDLK_f").type == 0)
				{
					context.OnInputEvent("SDLK_f", event);
					firedKeys.push_back("SDLK_f");
				}
				break;

			case SDLK_g:				
				if (context.GetKeyBinding("SDLK_g") != "" && context.GetKeyBoundEvent("SDLK_g").type == 0)
				{
					context.OnInputEvent("SDLK_g", event);
					firedKeys.push_back("SDLK_g");
				}
				break;

			case SDLK_h:				
				if (context.GetKeyBinding("SDLK_h") != "" && context.GetKeyBoundEvent("SDLK_h").type == 0)
				{
					context.OnInputEvent("SDLK_h", event);
					firedKeys.push_back("SDLK_h");
				}
				break;

			case SDLK_i:				
				if (context.GetKeyBinding("SDLK_i") != "" && context.GetKeyBoundEvent("SDLK_i").type == 0)
				{
					context.OnInputEvent("SDLK_i", event);
					firedKeys.push_back("SDLK_i");
				}
				break;

			case SDLK_j:				
				if (context.GetKeyBinding("SDLK_j") != "" && context.GetKeyBoundEvent("SDLK_j").type == 0)
				{
					context.OnInputEvent("SDLK_j", event);
					firedKeys.push_back("SDLK_j");
				}
				break;

			case SDLK_k:				
				if (context.GetKeyBinding("SDLK_k") != "" && context.GetKeyBoundEvent("SDLK_k").type == 0)
				{
					context.OnInputEvent("SDLK_k", event);
					firedKeys.push_back("SDLK_k");
				}
				break;

			case SDLK_l:				
				if (context.GetKeyBinding("SDLK_l") != "" && context.GetKeyBoundEvent("SDLK_l").type == 0)
				{
					context.OnInputEvent("SDLK_l", event);
					firedKeys.push_back("SDLK_l");
				}
				break;

			case SDLK_m:				
				if (context.GetKeyBinding("SDLK_m") != "" && context.GetKeyBoundEvent("SDLK_m").type == 0)
				{
					context.OnInputEvent("SDLK_m", event);
					firedKeys.push_back("SDLK_m");
				}
				break;

			case SDLK_n:				
				if (context.GetKeyBinding("SDLK_n") != "" && context.GetKeyBoundEvent("SDLK_n").type == 0)
				{
					context.OnInputEvent("SDLK_n", event);
					firedKeys.push_back("SDLK_n");
				}
				break;

			case SDLK_o:				
				if (context.GetKeyBinding("SDLK_o") != "" && context.GetKeyBoundEvent("SDLK_o").type == 0)
				{
					context.OnInputEvent("SDLK_o", event);
					firedKeys.push_back("SDLK_o");
				}
				break;

			case SDLK_p:				
				if (context.GetKeyBinding("SDLK_p") != "" && context.GetKeyBoundEvent("SDLK_p").type == 0)
				{
					context.OnInputEvent("SDLK_p", event);
					firedKeys.push_back("SDLK_p");
				}
				break;

			case SDLK_q:				
				if (context.GetKeyBinding("SDLK_q") != "" && context.GetKeyBoundEvent("SDLK_q").type == 0)
				{
					context.OnInputEvent("SDLK_q", event);
					firedKeys.push_back("SDLK_q");
				}
				break;

			case SDLK_r:			
				if (context.GetKeyBinding("SDLK_r") != "" && context.GetKeyBoundEvent("SDLK_r").type == 0)
				{
					context.OnInputEvent("SDLK_r", event);
					firedKeys.push_back("SDLK_r");
				}
				break;

			case SDLK_s:				
				if (context.GetKeyBinding("SDLK_s") != "" && context.GetKeyBoundEvent("SDLK_s").type == 0)
				{
					context.OnInputEvent("SDLK_s", event);
					firedKeys.push_back("SDLK_s");
				}
				break;

			case SDLK_t:				
				if (context.GetKeyBinding("SDLK_t") != "" && context.GetKeyBoundEvent("SDLK_t").type == 0)
				{
					context.OnInputEvent("SDLK_t", event);
					firedKeys.push_back("SDLK_t");
				}
				break;

			case SDLK_u:				
				if (context.GetKeyBinding("SDLK_u") != "" && context.GetKeyBoundEvent("SDLK_u").type == 0)
				{
					context.OnInputEvent("SDLK_u", event);
					firedKeys.push_back("SDLK_u");
				}
				break;

			case SDLK_v:				
				if (context.GetKeyBinding("SDLK_v") != "" && context.GetKeyBoundEvent("SDLK_v").type == 0)
				{
					context.OnInputEvent("SDLK_v", event);
					firedKeys.push_back("SDLK_v");
				}
				break;

			case SDLK_w:
				if (context.GetKeyBinding("SDLK_w") != "" && context.GetKeyBoundEvent("SDLK_w").type == 0)
				{
					context.OnInputEvent("SDLK_w", event);
					firedKeys.push_back("SDLK_w");
				}
				break;

			case SDLK_x:				
				if (context.GetKeyBinding("SDLK_x") != "" && context.GetKeyBoundEvent("SDLK_x").type == 0)
				{
					context.OnInputEvent("SDLK_x", event);
					firedKeys.push_back("SDLK_x");
				}
				break;

			case SDLK_y:				
				if (context.GetKeyBinding("SDLK_y") != "" && context.GetKeyBoundEvent("SDLK_y").type == 0)
				{
					context.OnInputEvent("SDLK_y", event);
					firedKeys.push_back("SDLK_y");
				}
				break;

			case SDLK_z:				
				if (context.GetKeyBinding("SDLK_z") != "" && context.GetKeyBoundEvent("SDLK_z").type == 0)
				{
					context.OnInputEvent("SDLK_z", event);
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
				if (context.GetKeyBinding("SDLK_SPACE") != "")
					context.ClearInputActionEvent("SDLK_SPACE");
				break;

			case SDLK_UP:				
				if (context.GetKeyBinding("SDLK_UP") != "")
					context.ClearInputActionEvent("SDLK_UP");
				break;

			case SDLK_DOWN:				
				if (context.GetKeyBinding("SDLK_DOWN") != "")
					context.ClearInputActionEvent("SDLK_DOWN");
				break;

			case SDLK_LEFT:				
				if (context.GetKeyBinding("SDLK_LEFT") != "")
					context.ClearInputActionEvent("SDLK_LEFT");
				break;

			case SDLK_RIGHT:				
				if (context.GetKeyBinding("SDLK_RIGHT") != "")
					context.ClearInputActionEvent("SDLK_RIGHT");
				break;

			case SDLK_a:				
				if (context.GetKeyBinding("SDLK_a") != "")
					context.ClearInputActionEvent("SDLK_a");
				break;

			case SDLK_b:				
				if (context.GetKeyBinding("SDLK_b") != "")
					context.ClearInputActionEvent("SDLK_b");
				break;

			case SDLK_c:
				if (context.GetKeyBinding("SDLK_c") != "")
					context.ClearInputActionEvent("SDLK_c");
				break;

			case SDLK_d:				
				if (context.GetKeyBinding("SDLK_d") != "")
					context.ClearInputActionEvent("SDLK_d");
				break;

			case SDLK_e:				
				if (context.GetKeyBinding("SDLK_e") != "")
					context.ClearInputActionEvent("SDLK_e");
				break;

			case SDLK_f:				
				if (context.GetKeyBinding("SDLK_f") != "")
					context.ClearInputActionEvent("SDLK_f");
				break;

			case SDLK_g:				
				if (context.GetKeyBinding("SDLK_g") != "")
					context.ClearInputActionEvent("SDLK_g");
				break;

			case SDLK_h:				
				if (context.GetKeyBinding("SDLK_h") != "")
					context.ClearInputActionEvent("SDLK_h");
				break;

			case SDLK_i:				
				if (context.GetKeyBinding("SDLK_i") != "")
					context.ClearInputActionEvent("SDLK_i");
				break;

			case SDLK_j:				
				if (context.GetKeyBinding("SDLK_j") != "")
					context.ClearInputActionEvent("SDLK_j");
				break;

			case SDLK_k:				
				if (context.GetKeyBinding("SDLK_k") != "")
					context.ClearInputActionEvent("SDLK_k");
				break;

			case SDLK_l:				
				if (context.GetKeyBinding("SDLK_l") != "")
					context.ClearInputActionEvent("SDLK_l");
				break;

			case SDLK_m:				
				if (context.GetKeyBinding("SDLK_m") != "")
					context.ClearInputActionEvent("SDLK_m");
				break;

			case SDLK_n:				
				if (context.GetKeyBinding("SDLK_n") != "")
					context.ClearInputActionEvent("SDLK_n");
				break;

			case SDLK_o:				
				if (context.GetKeyBinding("SDLK_o") != "")
					context.ClearInputActionEvent("SDLK_o");
				break;

			case SDLK_p:
				if (context.GetKeyBinding("SDLK_p") != "")
					context.ClearInputActionEvent("SDLK_p");
				break;

			case SDLK_q:				
				if (context.GetKeyBinding("SDLK_q") != "")
					context.ClearInputActionEvent("SDLK_q");
				break;

			case SDLK_r:				
				if (context.GetKeyBinding("SDLK_r") != "")
					context.ClearInputActionEvent("SDLK_r");
				break;

			case SDLK_s:				
				if (context.GetKeyBinding("SDLK_s") != "")
					context.ClearInputActionEvent("SDLK_s");
				break;

			case SDLK_t:				
				if (context.GetKeyBinding("SDLK_t") != "")
					context.ClearInputActionEvent("SDLK_t");
				break;

			case SDLK_u:				
				if (context.GetKeyBinding("SDLK_u") != "")
					context.ClearInputActionEvent("SDLK_u");
				break;

			case SDLK_v:				
				if (context.GetKeyBinding("SDLK_v") != "")
					context.ClearInputActionEvent("SDLK_v");
				break;

			case SDLK_w:				
				if (context.GetKeyBinding("SDLK_w") != "")
					context.ClearInputActionEvent("SDLK_w");
				break;

			case SDLK_x:			
				if (context.GetKeyBinding("SDLK_x") != "")
					context.ClearInputActionEvent("SDLK_x");
				break;

			case SDLK_y:			
				if (context.GetKeyBinding("SDLK_y") != "")
					context.ClearInputActionEvent("SDLK_y");
				break;

			case SDLK_z:				
				if (context.GetKeyBinding("SDLK_z") != "")
					context.ClearInputActionEvent("SDLK_z");
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
				if (context.GetKeyBinding("XInput_LeftJoystickX") != "")
					context.OnInputEvent("XInput_LeftJoystickX", event);
				break;
			case XInputAxis::LeftYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;				
				if (context.GetKeyBinding("XInput_LeftJoystickY") != "")
					context.OnInputEvent("XInput_LeftJoystickY", event);
				break;
			case XInputAxis::RightXAxis:
				// Left of dead zone or Right of dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;			
				if (context.GetKeyBinding("XInput_RightJoystick") != "")
					context.OnInputEvent("XInput_RightJoystick", event);
				break;
			case XInputAxis::RightYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;			
				if (context.GetKeyBinding("XInput_RightJoystick") != "")
					context.OnInputEvent("XInput_RightJoystick", event);
				break;
			case XInputAxis::LT:				
				if (context.GetKeyBinding("XInput_LT") != "")
					context.OnInputEvent("XInput_LT", event);
				break;
			case XInputAxis::RT:				
				if (context.GetKeyBinding("XInput_RT") != "")
					context.OnInputEvent("XInput_RT", event);
				break;
			}
		}
		// Buttons Down
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			switch (event.jbutton.button)
			{
			case XInputButtons::A:			
				if (context.GetKeyBinding("XInput_A") != "")
				{
					context.OnInputEvent("XInput_A", event);
					firedKeys.push_back("XInput_A");
				}

				break;
			case XInputButtons::B:				
				if (context.GetKeyBinding("XInput_B") != "")
				{
					context.OnInputEvent("XInput_B", event);
					firedKeys.push_back("XInput_B");
				}
				break;
			case XInputButtons::X:				
				if (context.GetKeyBinding("XInput_X") != "")
				{
					context.OnInputEvent("XInput_X", event);
					firedKeys.push_back("XInput_X");
				}
				break;
			case XInputButtons::Y:				
				if (context.GetKeyBinding("XInput_Y") != "")
				{
					context.OnInputEvent("XInput_Y", event);
					firedKeys.push_back("XInput_Y");
				}
				break;
			case XInputButtons::LB:				
				if (context.GetKeyBinding("XInput_LB") != "")
				{
					context.OnInputEvent("XInput_LB", event);
					firedKeys.push_back("XInput_LB");
				}
				break;
			case XInputButtons::RB:				
				if (context.GetKeyBinding("XInput_RB") != "")
				{
					context.OnInputEvent("XInput_RB", event);
					firedKeys.push_back("XInput_RB");
				}
				break;
			case XInputButtons::ScreenShot:				
				if (context.GetKeyBinding("XInput_ScreenShot") != "")
				{
					context.OnInputEvent("XInput_ScreenShot", event);
					firedKeys.push_back("XInput_ScreenShot");
				}
				break;
			case XInputButtons::Start:				
				if (context.GetKeyBinding("XInput_Start") != "")
				{
					context.OnInputEvent("XInput_Start", event);
					firedKeys.push_back("XInput_Start");
				}
				break;
			case XInputButtons::LS:			
				if (context.GetKeyBinding("XInput_LS") != "")
				{
					context.OnInputEvent("XInput_LS", event);
					firedKeys.push_back("XInput_LS");
				}
				break;
			case XInputButtons::RS:			
				if (context.GetKeyBinding("XInput_RS") != "")
				{
					context.OnInputEvent("XInput_RS", event);
					firedKeys.push_back("XInput_RS");
				}
				break;
			case XInputButtons::Home:			
				if (context.GetKeyBinding("XInput_Home") != "")
				{
					context.OnInputEvent("XInput_Home", event);
					firedKeys.push_back("XInput_Home");
				}
				break;
			case XInputButtons::Tray:				
				if (context.GetKeyBinding("XInput_Tray") != "")
				{
					context.OnInputEvent("XInput_Tray", event);
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
				if (context.GetKeyBinding("XInput_A") != "")
					context.ClearInputActionEvent("XInput_A");
				break;
			case XInputButtons::B:				
				if (context.GetKeyBinding("XInput_B") != "")
					context.ClearInputActionEvent("XInput_B");
				break;
			case XInputButtons::X:				
				if (context.GetKeyBinding("XInput_X") != "")
					context.ClearInputActionEvent("XInput_X");
				break;
			case XInputButtons::Y:				
				if (context.GetKeyBinding("XInput_Y") != "")
					context.ClearInputActionEvent("XInput_Y");
				break;
			case XInputButtons::LB:
				if (context.GetKeyBinding("XInput_LB") != "")
					context.ClearInputActionEvent("XInput_LB");
				break;
			case XInputButtons::RB:
				if (context.GetKeyBinding("XInput_RB") != "")
					context.ClearInputActionEvent("XInput_RB");
				break;
			case XInputButtons::ScreenShot:
				if (context.GetKeyBinding("XInput_ScreenShot") != "")
					context.ClearInputActionEvent("XInput_ScreenShot");
				break;
			case XInputButtons::Start:
				if (context.GetKeyBinding("XInput_Start") != "")
					context.ClearInputActionEvent("XInput_Start");
				break;
			case XInputButtons::LS:
				if (context.GetKeyBinding("XInput_LS") != "")
					context.ClearInputActionEvent("XInput_LS");
				break;
			case XInputButtons::RS:
				if (context.GetKeyBinding("XInput_RS") != "")
					context.ClearInputActionEvent("XInput_RS");
				break;
			case XInputButtons::Home:
				if (context.GetKeyBinding("XInput_Home") != "")
					context.ClearInputActionEvent("XInput_Home");
				break;
			case XInputButtons::Tray:
				if (context.GetKeyBinding("XInput_Tray") != "")
					context.ClearInputActionEvent("XInput_Tray");
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


	// TileSet / TileMap Management
	void CreateNewTileSetFile(std::string fileName, std::string path)
	{
		std::string filePath = "";		
		TileSet tileSet = TileSet();

		if (path == "")
			filePath = GetDir("tileSets") + "/" + fileName + ".tls";
		else
			filePath = path + "/" + fileName + ".tls";

		tileSet.SetTileSetPath(filePath);
		tileSet.SetName(fileName);
		SaveTileSet(tileSet);
		InitializeTileSets();
	}

	void SaveTileSet(TileSet tileSet)
	{
		std::string texturePath = tileSet.GetTexturePath();
		std::string filepath = tileSet.GetTileSetPath();		

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filepath);

		// Delete old contents of the file
		file_obj.open(filepath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(filepath, std::ios::app);

		std::string data = tileSet.GetData();

		// Declare array json object for Tiles
		json tileArray = json::array();

		for (int index : tileSet.GetTileSetIndices())
		{
			// Get tile data
			json jsonData = {
				{ "index", index }
			};

			// Dumped json object with required data for saving
			std::string tileData = jsonData.dump();

			// Save to the json array
			tileArray.push_back(json::parse(tileData));
		}

		// Create TileSet Json data object
		json tileSetJson = json::object({
			{ "tileSetData", json::parse(data) },
			{ "tiles", tileArray }
		});

		// Add the GameObjects object contents to the file
		file_obj << tileSetJson.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void InitializeTileSets()
	{
		F_TileSets.clear();

		std::vector<std::string> tileSetFiles = std::vector<std::string>();
		tileSetFiles = FindAllFilesWithExtension(GetDir("projectDir"), ".tls");

		for (std::string path : tileSetFiles)
		{
			// Create a new context to save the loaded keybindings to
			TileSet tileSet = TileSet();

			json tileSetJson = LoadFileData(path);
			if (tileSetJson != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto tileSetData = tileSetJson["tileSetData"];

				tileSet.SetName(tileSetData["name"]);
				tileSet.SetTileSetPath(tileSetData["tileSetPath"]);
				tileSet.SetTexturePath(tileSetData["texturePath"]);
				tileSet.SetTileWidth(tileSetData["tileWidth"]);
				tileSet.SetTileHeight(tileSetData["tileHeight"]);

				std::vector<int> indices;
				for (auto tile : tileSetJson["tiles"])
				{
					int index = CheckJsonInt(tile, "index", "TileSet tiles");
					// Fix this later to return a bool and change the value of the int within the parameters
					if (index != -1)
					{
						indices.push_back(index);
					}
				}
				tileSet.SetTileSetIndices(indices);

				// Add context to context managing vector
				F_TileSets.push_back(tileSet);
			}
		}
	}

	TileSet* GetTileSet(std::string tileSetName) 
	{
		for (std::vector<TileSet>::iterator iter = F_TileSets.begin(); iter != F_TileSets.end();)
		{
			if (iter->GetName() == tileSetName)
				return &(*iter);

			iter++;
		}

		return nullptr;
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
	void CreateNewAnimationFile(std::string filename, std::string path)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::string filePath = "";
		// Delete old contents of the file

		if (path == "")
			filePath = GetDir("animations") + filename + ".anm";
		else
			filePath = path + "/" + filename + ".anm";
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(filePath, std::ios::app);

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

	void SaveAnimationData(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path)
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
	void LogError(std::string line, std::string from)
	{
		F_Logger.LogString("ERROR : " + line, from);
	}

	void LogString(std::string line, std::string from)
	{
		F_Logger.LogString(line, from);
	}

	void LogFloat(float var, std::string line, std::string from)
	{
		F_Logger.LogFloat(var, line, from);
	}

	void LogInt(int var, std::string line, std::string from)
	{
		F_Logger.LogInt(var, line, from);
	}

	void LogVector2(Vector2 vector, std::string line, std::string from)
	{
		F_Logger.LogVector2(vector, line, from);
	}

	void LogSeparator()
	{
		F_Logger.LogSeparator();
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
		sSelectedFile = sFilePath.substr(slash);
		//  SUCCESS, CLEAN UP

		std::string relativePath = MakePathRelative(sFilePath);

		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();

	
		return relativePath;
	}

	std::string GetFilenameFromPath(std::string path, bool _keepExtension)
	{
		std::string finalName = "";

		if (path != "")
		{
			//  FORMAT STRING
			const size_t slash = path.find_last_of("/\\");
			std::string wholeFilename = path.substr(slash + 1);
			const size_t dot1 = wholeFilename.find_last_of(".");
			std::string extension1 = wholeFilename.substr(dot1);

			if (!_keepExtension)
			{
				finalName = wholeFilename.substr(0, wholeFilename.size() - extension1.size());
				// For scripting files (.scp.lua) with two extensions
				const size_t dot2 = finalName.find_last_of(".");
				if (dot2 < 100)
				{
					std::string extension2 = finalName.substr(dot2);
					finalName = finalName.substr(0, finalName.size() - extension2.size());
				}
			}
			else
				finalName = wholeFilename;
		}

		return finalName;
	}

	//  Removes absolute path from the beginning of the selected filepath up to just after "FlatEngine"
	std::string MakePathRelative(std::string filepath)
	{
		std::string relativePath = "";
		std::string root = GetDir("root");
		size_t rootDirIndex;

		if (filepath != "")
		{
			if (root != "")
				rootDirIndex = filepath.find(root) + 10;
			else
				rootDirIndex = 0;

			if (rootDirIndex < 1000 && rootDirIndex != 0)
				relativePath = ".." + filepath.substr(rootDirIndex);
			else
				relativePath = filepath;
		}

		return relativePath;
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

	json LoadFileData(std::string filepath)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filepath);

		// Open file in in mode
		file_obj.open(filepath, std::ios::in);

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


	bool DoesFileExist(std::string filepath)
	{
		if (filepath != "" && std::filesystem::exists(filepath))
			return true;
		else
			return false;
	}

	bool FilepathHasExtension(std::string filepath, std::string extension)
	{
		const size_t dot = filepath.find_last_of(".");
		std::string actualExtension = filepath.substr(dot);

		return actualExtension == extension;
	}

	void DeleteFileUsingPath(std::string filepath)
	{
		if (filepath != "" && DoesFileExist(filepath))
		{
			std::filesystem::remove_all(filepath);
		}
	}

	std::vector<std::string> FindAllFilesWithExtension(std::string dirPath, std::string extension)
	{
		std::vector<std::string> files;

		for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
		{
			if (p.path().extension() == extension || p.path().string().find(extension) != std::string::npos)
				files.push_back(p.path().string());				
		}

		return files;
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
		float value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}

	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
	{
		int value = -1;
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
					Transform* newTransform = loadedObject.AddTransform(id, _isActive, _isCollapsed);
					float rotation = CheckJsonFloat(componentJson, "rotation", objectName);
					newTransform->SetOrigin(Vector2(CheckJsonFloat(componentJson, "xOrigin", objectName), CheckJsonFloat(componentJson, "yOrigin", objectName)));
					newTransform->SetInitialPosition(Vector2(CheckJsonFloat(componentJson, "xPos", objectName), CheckJsonFloat(componentJson, "yPos", objectName)));
					newTransform->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					newTransform->SetRotation(rotation);
					objectRotation = rotation;
				}
				else if (type == "Sprite")
				{
					Sprite* newSprite = loadedObject.AddSprite(id, _isActive, _isCollapsed);
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
					Camera* newCamera = loadedObject.AddCamera(id, _isActive, _isCollapsed);
					bool _isPrimaryCamera = CheckJsonBool(componentJson, "_isPrimaryCamera", objectName);
					newCamera->SetDimensions(CheckJsonFloat(componentJson, "width", objectName), CheckJsonFloat(componentJson, "height", objectName));
					newCamera->SetPrimaryCamera(_isPrimaryCamera);
					newCamera->SetZoom(CheckJsonFloat(componentJson, "zoom", objectName));
					newCamera->SetFrustrumColor(Vector4(
						CheckJsonFloat(componentJson, "frustrumRed", objectName),
						CheckJsonFloat(componentJson, "frustrumGreen", objectName),
						CheckJsonFloat(componentJson, "frustrumBlue", objectName),
						CheckJsonFloat(componentJson, "frustrumAlpha", objectName)
					));
					newCamera->SetShouldFollow(CheckJsonBool(componentJson, "_follow", objectName));
					newCamera->SetFollowSmoothing(CheckJsonFloat(componentJson, "followSmoothing", objectName));
					newCamera->SetFollowing(CheckJsonLong(componentJson, "following", objectName));
				}
				else if (type == "Script")
				{
					Script* newScript = loadedObject.AddScript(id, _isActive, _isCollapsed);
					newScript->SetAttachedScript(CheckJsonString(componentJson, "attachedScript", objectName));
				}
				else if (type == "Button")
				{
					Button* newButton = loadedObject.AddButton(id, _isActive, _isCollapsed);
					newButton->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newButton->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newButton->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
				}
				else if (type == "Canvas")
				{
					Canvas* newCanvas = loadedObject.AddCanvas(id, _isActive, _isCollapsed);
					newCanvas->SetDimensions(CheckJsonFloat(componentJson, "canvasWidth", objectName), CheckJsonFloat(componentJson, "canvasHeight", objectName));
					newCanvas->SetLayerNumber(CheckJsonInt(componentJson, "layerNumber", objectName));
					newCanvas->SetBlocksLayers(CheckJsonBool(componentJson, "_blocksLayers", objectName));
				}
				else if (type == "Animation")
				{
					Animation* newAnimation = loadedObject.AddAnimation(id, _isActive, _isCollapsed);
					newAnimation->SetAnimationPath(CheckJsonString(componentJson, "path", objectName));
				}
				else if (type == "Audio")
				{
					Audio* newAudio = loadedObject.AddAudio(id, _isActive, _isCollapsed);				

					// Get Sounds data
					if (JsonContains(componentJson, "soundData", objectName))
					{
						for (int sound = 0; sound < componentJson["soundData"].size(); sound++)
						{
							json tileJson = componentJson["soundData"][sound];
							std::string path = CheckJsonString(tileJson, "path", objectName);
							std::string name = CheckJsonString(tileJson, "name", objectName);
							bool b_isMusic = CheckJsonBool(tileJson, "b_isMusic", objectName);							

							newAudio->AddSound(name, path, b_isMusic);
						}
					}
				}
				else if (type == "Text")
				{
					Text* newText = loadedObject.AddText(id, _isActive, _isCollapsed);
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
					CharacterController* newCharacterController = loadedObject.AddCharacterController(id, _isActive, _isCollapsed);
					newCharacterController->SetMaxAcceleration(CheckJsonFloat(componentJson, "maxAcceleration", objectName));
					newCharacterController->SetMaxSpeed(CheckJsonFloat(componentJson, "maxSpeed", objectName));
					newCharacterController->SetAirControl(CheckJsonFloat(componentJson, "airControl", objectName));
				}
				else if (type == "BoxCollider")
				{
					BoxCollider* newBoxCollider = loadedObject.AddBoxCollider(id, _isActive, _isCollapsed);
					newBoxCollider->SetTileMapCollider(false);
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
					CircleCollider* newCircleCollider = loadedObject.AddCircleCollider(id, _isActive, _isCollapsed);
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
					RigidBody* newRigidBody = loadedObject.AddRigidBody(id, _isActive, _isCollapsed);
					newRigidBody->SetMass(CheckJsonFloat(componentJson, "mass", objectName));
					newRigidBody->SetAngularDrag(CheckJsonFloat(componentJson, "angularDrag", objectName));
					newRigidBody->SetGravity(CheckJsonFloat(componentJson, "gravity", objectName));
					newRigidBody->SetFallingGravity(CheckJsonFloat(componentJson, "fallingGravity", objectName));
					newRigidBody->SetFriction(CheckJsonFloat(componentJson, "friction", objectName));
					newRigidBody->SetWindResistance(CheckJsonFloat(componentJson, "windResistance", objectName));
					newRigidBody->SetEquilibriumForce(CheckJsonFloat(componentJson, "equilibriumForce", objectName));
					newRigidBody->SetTerminalVelocity(CheckJsonFloat(componentJson, "terminalVelocity", objectName));
					newRigidBody->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newRigidBody->SetTorquesAllowed(CheckJsonBool(componentJson, "_allowTorques", objectName));
				}
				else if (type == "TileMap")
				{
					TileMap* newTileMap = loadedObject.AddTileMap(id, _isActive, _isCollapsed);
					newTileMap->SetWidth(CheckJsonInt(componentJson, "width", objectName));
					newTileMap->SetHeight(CheckJsonInt(componentJson, "height", objectName));
					newTileMap->SetTileWidth(CheckJsonInt(componentJson, "tileWidth", objectName));
					newTileMap->SetTileHeight(CheckJsonInt(componentJson, "tileHeight", objectName));

					// Get used TileSet names
					if (JsonContains(componentJson, "tileSets", objectName))
					{
						for (int tileSet = 0; tileSet < componentJson["tileSets"].size(); tileSet++)
						{
							json tileSetJson = componentJson["tileSets"][tileSet];
							newTileMap->AddTileSet(CheckJsonString(tileSetJson, "name", objectName));
						}
					}
					// Get Tile data
					if (JsonContains(componentJson, "tiles", objectName))
					{
						for (int tile = 0; tile < componentJson["tiles"].size(); tile++)
						{
							json tileJson = componentJson["tiles"][tile];
							float x = CheckJsonFloat(tileJson, "tileCoordX", objectName);
							float y = CheckJsonFloat(tileJson, "tileCoordY", objectName);
							std::string tileSetName = CheckJsonString(tileJson, "tileSetName", objectName);
							int tileSetIndex = CheckJsonInt(tileJson, "tileSetIndex", objectName);

							if (tileSetName != "" && tileSetIndex != -1)
							{
								newTileMap->SetTile(Vector2(x, y), GetTileSet(tileSetName), tileSetIndex);
							}
						}
					}
					// Get Collision Area data
					if (JsonContains(componentJson, "collisionAreas", objectName))
					{
						for (int collisionArea = 0; collisionArea < componentJson["collisionAreas"].size(); collisionArea++)
						{
							json colliderAreaJson = componentJson["collisionAreas"][collisionArea];
							json colliderDataJson = colliderAreaJson["areaData"];
							std::string collisionAreaName = CheckJsonString(colliderAreaJson, "name", objectName);
							std::vector<std::pair<Vector2, Vector2>> collisionCoordBuffer;

							for (int colArea = 0; colArea < colliderDataJson.size(); colArea++)
							{											
								Vector2 startCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "startCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "startCoordY", objectName));
								Vector2 endCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "endCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "endCoordY", objectName));
								std::pair<Vector2, Vector2> colPair = { startCoord, endCoord };
								collisionCoordBuffer.push_back(colPair);
							}

							if (collisionCoordBuffer.size() > 0)
							{
								newTileMap->SetCollisionAreaValues(collisionAreaName, collisionCoordBuffer);
							}										
						}
					}
				}
			}
		}

		// Update the moment of inertia if applicable
		if (loadedObject.GetRigidBody() != nullptr)
			loadedObject.GetRigidBody()->UpdateI();

		loadedObject.SetName(objectName);
		loadedObject.SetActive(_isActive);

		// Set children after because they may not be created yet and SetActive() calls child objects
		for (int c = 0; c < objectJson["children"].size(); c++)
		{
			loadedObject.AddChild(loadedChildrenIDs[c]);
		}

		return loadedObject;
	}
}

// ImGui cheat sheet
// Border around object
//auto wPos = ImGui::GetWindowPos();
//auto wSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
//ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, FL::GetColor32("componentBorder"), 2);

