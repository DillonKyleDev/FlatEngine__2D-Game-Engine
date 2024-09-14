#include "FlatEngine.h"
#include "WindowManager.h"
#include "MappingContext.h"
#include "PrefabManager.h"
#include "Logger.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "Vector4.h"
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
#include "Canvas.h"
#include "GameObject.h"

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
	bool F_b_closeProgram = false;

	std::shared_ptr<WindowManager> F_Window = std::make_shared<WindowManager>();

	std::string F_RuntimeDirectoriesLuaFilepath = "..\\engine\\scripts\\RuntimeDirectories.lua";
	std::string F_EditorDirectoriesLuaFilepath = "..\\engine\\scripts\\EditorDirectories.lua";
	std::shared_ptr<Application> F_Application = std::make_shared<Application>();
	AssetManager F_AssetManager = AssetManager();
	std::vector<std::string> F_selectedFiles = std::vector<std::string>();

	F_CURSOR_MODE F_CursorMode = F_CURSOR_MODE::TRANSLATE;
	bool F_b_sceneViewFocused = false;
	


	bool F_b_loadNewScene = false;
	std::string F_sceneToBeLoaded = "";

	std::vector<SDL_Joystick*> F_gamepads = std::vector<SDL_Joystick*>();	

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
	SceneManager F_SceneManager = SceneManager();	
	Sound F_SoundController = Sound();
	std::vector<MappingContext> F_MappingContexts = std::vector<MappingContext>();
	std::string F_selectedMappingContextName = "";
	TTF_Font* F_fontCinzel;
	std::shared_ptr<PrefabManager> F_PrefabManager = std::make_shared<PrefabManager>();
	std::vector<TileSet> F_TileSets = std::vector<TileSet>();
	std::string F_selectedTileSetToEdit = "- none -";
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
		F_fontCinzel = TTF_OpenFont("C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine3D\\FlatEngine-Core\\Source\\assets\\fonts\\Cinzel\\Cinzel-Black.ttf", 46);
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
		// Changing the scale here because sprites render too large
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
			printf("SDL initialized... - Video - Audio - Joystick -\n");

			//Set texture filtering to linear
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
			{
				printf("Warning: Linear texture filtering not enabled!\n");
			}

			char t[] = "FlatEngine";
			char* title = &(t[0]);

			//Initialize Window::window
			if (F_Window->Init(title, windowWidth, windowHeight))
			{
				printf("Window initialized...\n");

				//Initialize SDL_image for png loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					printf("SDL_image initialized...\n");
					//Initialize SDL_ttf for text rendering
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
					else
					{
						printf("TTF_Fonts initialized...\n");
						//Initialize SDL_mixer
						if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
						{
							printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
							success = false;
						}
						else
						{
							Mix_AllocateChannels(100);							   // Sets number of individual audios that can play at once
							printf("SDL_mixer initialized...\n");

							InitLua();
							printf("Lua initialized...\n");

							F_AssetManager.CollectDirectories(dirType);			   // Collect important directories and file paths from Directories.lua
							F_AssetManager.CollectColors();						   // Collect global colors from Colors.lua
							F_AssetManager.CollectTextures();				       // Collect and create Texture icons from Textures.lua
							SetupImGui();										   // Setup ImGui for use in the prompt for Directories.lua location
							SetImGuiColors();									   // Use the collected colors to style ImGui elements
							RetrieveLuaScriptPaths();
							InitializeTileSets();

							printf("Engine Assets initialized...\n");
							printf("System Ready...\n");

							LogSeparator();
							LogString("System ready. Begin logging...");
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

		ImGui_ImplSDL2_InitForSDLRenderer(F_Window->GetWindow(), F_Window->GetRenderer());
		ImGui_ImplSDLRenderer2_Init(F_Window->GetRenderer());
		SetImGuiColors();  // Colors will not be loaded yet, but they will obtain the default color given by GetColor();
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
		for (SDL_Joystick* gamepad : F_gamepads)
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
	
		F_b_closeProgram = true;
	}


	void ManageControllers()
	{
		static int controllersConnected = 0;
		if (SDL_NumJoysticks() != controllersConnected)
		{
			// Clean up old gamepads
			for (SDL_Joystick* gamepad : F_gamepads)
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
					F_gamepads.push_back(gamepad);
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
		{
			LoadScene(newProject.GetLoadedScenePath());
		}
		else
		{
			CreateNewScene();
		}

		// Set loaded project
		SetLoadedProject(newProject);
	}

	void BuildProject()
	{
		if (F_LoadedProject.GetBuildPath() != "")
		{
			try
			{
				std::filesystem::copy("..\\FlatEngine-Core", F_LoadedProject.GetBuildPath() + "\\Core", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy FlatEngine-Core : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\FlatEngine-Runtime", F_LoadedProject.GetBuildPath() + "\\Runtime", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy FlatEngine-Runtime : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\assets", F_LoadedProject.GetBuildPath() + "\\assets", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy assets : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\engine", F_LoadedProject.GetBuildPath() + "\\engine", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy engine dependencies: ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\intermediates", F_LoadedProject.GetBuildPath() + "\\intermediates", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy intermediates: ");
				LogError(e.what());
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

	// Waits until EndImGuiRender() has been called in Application.cpp to load the next scene
	void QueueLoadScene(std::string scenePath)
	{
		F_b_loadNewScene = true;
		F_sceneToBeLoaded = scenePath;
	}

	void LoadScene(std::string filepath)
	{
		// Stop any playing music
		F_SoundController.StopMusic();

		if (F_SceneManager.LoadScene(filepath))
		{
			if (F_SceneManager.GetLoadedScene() != nullptr)
			{
				F_SceneManager.SaveAnimationPreviewObjects();
			}

			F_SceneManager.LoadAnimationPreviewObjects();
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
			filePath = GetDir("scenes") + "\\" + filename + ".scn";
		else
			filePath = path + "\\" + filename + ".scn";

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

	GameObject* CreateGameObject(long parentID, long myID)
	{
		return GetLoadedScene()->CreateGameObject(parentID, myID);
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
			filePath = GetDir("mappingContexts") + "\\" + fileName + ".mpc";
		else
			filePath = path + "\\" + fileName + ".mpc";

		newContext.SetPath(filePath);
		newContext.SetName(fileName);
		SaveMappingContext(filePath, newContext);
		InitializeMappingContexts();
	}

	GameObject* CreateAssetUsingFilePath(std::string filePath, Vector2 position)
	{
		std::string extension = std::filesystem::path(filePath).extension().string();

		// Scene file
		if (extension == ".png")
		{
			GameObject* newObject = CreateGameObject();
			newObject->SetName(GetFilenameFromPath(filePath) + "(" + std::to_string(newObject->GetID()) + ")");
			newObject->GetTransform()->SetPosition(position);
			newObject->AddSprite()->SetTexture(filePath);
			return newObject;
		}
		else if (extension == ".prf")
		{
			return Instantiate(GetFilenameFromPath(filePath), position);
		}
		else
		{
			return nullptr;
		}
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
				newContext.AddKeyBinding("Keyboard_up", mappings["Keyboard_up"]);
				newContext.AddKeyBinding("Keyboard_down", mappings["Keyboard_down"]);
				newContext.AddKeyBinding("Keyboard_left", mappings["Keyboard_left"]);
				newContext.AddKeyBinding("Keyboard_right", mappings["Keyboard_right"]);
				// Letters
				newContext.AddKeyBinding("Keyboard_a", mappings["Keyboard_a"]);
				newContext.AddKeyBinding("Keyboard_b", mappings["Keyboard_b"]);
				newContext.AddKeyBinding("Keyboard_c", mappings["Keyboard_c"]);
				newContext.AddKeyBinding("Keyboard_d", mappings["Keyboard_d"]);
				newContext.AddKeyBinding("Keyboard_e", mappings["Keyboard_e"]);
				newContext.AddKeyBinding("Keyboard_f", mappings["Keyboard_f"]);
				newContext.AddKeyBinding("Keyboard_g", mappings["Keyboard_g"]);
				newContext.AddKeyBinding("Keyboard_h", mappings["Keyboard_h"]);
				newContext.AddKeyBinding("Keyboard_i", mappings["Keyboard_i"]);
				newContext.AddKeyBinding("Keyboard_j", mappings["Keyboard_j"]);
				newContext.AddKeyBinding("Keyboard_k", mappings["Keyboard_k"]);
				newContext.AddKeyBinding("Keyboard_l", mappings["Keyboard_l"]);
				newContext.AddKeyBinding("Keyboard_m", mappings["Keyboard_m"]);
				newContext.AddKeyBinding("Keyboard_n", mappings["Keyboard_n"]);
				newContext.AddKeyBinding("Keyboard_o", mappings["Keyboard_o"]);
				newContext.AddKeyBinding("Keyboard_p", mappings["Keyboard_p"]);
				newContext.AddKeyBinding("Keyboard_q", mappings["Keyboard_q"]);
				newContext.AddKeyBinding("Keyboard_r", mappings["Keyboard_r"]);
				newContext.AddKeyBinding("Keyboard_s", mappings["Keyboard_s"]);
				newContext.AddKeyBinding("Keyboard_t", mappings["Keyboard_t"]);
				newContext.AddKeyBinding("Keyboard_u", mappings["Keyboard_u"]);
				newContext.AddKeyBinding("Keyboard_v", mappings["Keyboard_v"]);
				newContext.AddKeyBinding("Keyboard_w", mappings["Keyboard_w"]);
				newContext.AddKeyBinding("Keyboard_x", mappings["Keyboard_x"]);
				newContext.AddKeyBinding("Keyboard_y", mappings["Keyboard_y"]);
				newContext.AddKeyBinding("Keyboard_z", mappings["Keyboard_z"]);

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
					if (event.window.windowID == SDL_GetWindowID(F_Window->GetWindow()))
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
				case SDLK_1:
					F_CursorMode = F_CURSOR_MODE::TRANSLATE;
					break;

				case SDLK_2:
					F_CursorMode = F_CURSOR_MODE::SCALE;
					break;

				case SDLK_3:
					F_CursorMode = F_CURSOR_MODE::ROTATE;
					break;

				case SDLK_4:
					F_CursorMode = F_CURSOR_MODE::TILE_BRUSH;
					break;

				case SDLK_5:
					F_CursorMode = F_CURSOR_MODE::TILE_ERASE;
					break;

				case SDLK_6:
					F_CursorMode = F_CURSOR_MODE::TILE_COLLIDER_DRAW;
					break;

				case SDLK_7:
					F_CursorMode = F_CURSOR_MODE::TILE_MOVE;
					break;

				case SDLK_8:
					F_CursorMode = F_CURSOR_MODE::TILE_MULTISELECT;
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
			{
				if (context.GetKeyBinding("Keyboard_space") != "" && context.GetKeyBoundEvent("Keyboard_space").type == 0)
				{
					context.OnInputEvent("Keyboard_space", event);
					firedKeys.push_back("Keyboard_space");
				}
				break;
			}
			case SDLK_ESCAPE:
			{
				if (context.GetKeyBinding("Keyboard_escape") != "" && context.GetKeyBoundEvent("Keyboard_escape").type == 0)
				{
					context.OnInputEvent("Keyboard_escape", event);
					firedKeys.push_back("Keyboard_escape");
				}
				break;
			}
			case SDLK_UP:			
			{
				if (context.GetKeyBinding("Keyboard_up") != "" && context.GetKeyBoundEvent("Keyboard_up").type == 0)
				{
					context.OnInputEvent("Keyboard_up", event);
					firedKeys.push_back("Keyboard_up");
				}
				break;
			}
			case SDLK_DOWN:
			{
				if (context.GetKeyBinding("Keyboard_down") != "" && context.GetKeyBoundEvent("Keyboard_down").type == 0)
				{
					context.OnInputEvent("Keyboard_down", event);
					firedKeys.push_back("Keyboard_down");
				}
				break;
			}
			case SDLK_LEFT:
			{
				if (context.GetKeyBinding("Keyboard_left") != "" && context.GetKeyBoundEvent("Keyboard_left").type == 0)
				{
					context.OnInputEvent("Keyboard_left", event);
					firedKeys.push_back("Keyboard_left");
				}
				break;
			}
			case SDLK_RIGHT:				
			{
				if (context.GetKeyBinding("Keyboard_right") != "" && context.GetKeyBoundEvent("Keyboard_right").type == 0)
				{
					context.OnInputEvent("Keyboard_right", event);
					firedKeys.push_back("Keyboard_right");
				}
				break;
			}
			case SDLK_a:				
			{
				if (context.GetKeyBinding("Keyboard_a") != "" && context.GetKeyBoundEvent("Keyboard_a").type == 0)
				{
					context.OnInputEvent("Keyboard_a", event);
					firedKeys.push_back("Keyboard_a");
				}
				break;
			}
			case SDLK_b:				
			{
				if (context.GetKeyBinding("Keyboard_b") != "" && context.GetKeyBoundEvent("Keyboard_b").type == 0)
				{
					context.OnInputEvent("Keyboard_b", event);
					firedKeys.push_back("Keyboard_b");
				}
				break;
			}
			case SDLK_c:				
			{
				if (context.GetKeyBinding("Keyboard_c") != "" && context.GetKeyBoundEvent("Keyboard_c").type == 0)
				{
					context.OnInputEvent("Keyboard_c", event);
					firedKeys.push_back("Keyboard_c");
				}
				break;
			}
			case SDLK_d:				
			{
				if (context.GetKeyBinding("Keyboard_d") != "" && context.GetKeyBoundEvent("Keyboard_d").type == 0)
				{
					context.OnInputEvent("Keyboard_d", event);
					firedKeys.push_back("Keyboard_d");
				}
				break;
			}
			case SDLK_e:				
			{
				if (context.GetKeyBinding("Keyboard_e") != "" && context.GetKeyBoundEvent("Keyboard_e").type == 0)
				{
					context.OnInputEvent("Keyboard_e", event);
					firedKeys.push_back("Keyboard_e");
				}
				break;
			}
			case SDLK_f:				
			{
				if (context.GetKeyBinding("Keyboard_f") != "" && context.GetKeyBoundEvent("Keyboard_f").type == 0)
				{
					context.OnInputEvent("Keyboard_f", event);
					firedKeys.push_back("Keyboard_f");
				}
				break;
			}
			case SDLK_g:				
			{
				if (context.GetKeyBinding("Keyboard_g") != "" && context.GetKeyBoundEvent("Keyboard_g").type == 0)
				{
					context.OnInputEvent("Keyboard_g", event);
					firedKeys.push_back("Keyboard_g");
				}
				break;
			}
			case SDLK_h:				
			{
				if (context.GetKeyBinding("Keyboard_h") != "" && context.GetKeyBoundEvent("Keyboard_h").type == 0)
				{
					context.OnInputEvent("Keyboard_h", event);
					firedKeys.push_back("Keyboard_h");
				}
				break;
			}
			case SDLK_i:				
			{
				if (context.GetKeyBinding("Keyboard_i") != "" && context.GetKeyBoundEvent("Keyboard_i").type == 0)
				{
					context.OnInputEvent("Keyboard_i", event);
					firedKeys.push_back("Keyboard_i");
				}
				break;
			}
			case SDLK_j:				
			{
				if (context.GetKeyBinding("Keyboard_j") != "" && context.GetKeyBoundEvent("Keyboard_j").type == 0)
				{
					context.OnInputEvent("Keyboard_j", event);
					firedKeys.push_back("Keyboard_j");
				}
				break;
			}
			case SDLK_k:				
			{
				if (context.GetKeyBinding("Keyboard_k") != "" && context.GetKeyBoundEvent("Keyboard_k").type == 0)
				{
					context.OnInputEvent("Keyboard_k", event);
					firedKeys.push_back("Keyboard_k");
				}
				break;
			}
			case SDLK_l:				
			{
				if (context.GetKeyBinding("Keyboard_l") != "" && context.GetKeyBoundEvent("Keyboard_l").type == 0)
				{
					context.OnInputEvent("Keyboard_l", event);
					firedKeys.push_back("Keyboard_l");
				}
				break;
			}
			case SDLK_m:				
			{
				if (context.GetKeyBinding("Keyboard_m") != "" && context.GetKeyBoundEvent("Keyboard_m").type == 0)
				{
					context.OnInputEvent("Keyboard_m", event);
					firedKeys.push_back("Keyboard_m");
				}
				break;
			}
			case SDLK_n:
			{
				if (context.GetKeyBinding("Keyboard_n") != "" && context.GetKeyBoundEvent("Keyboard_n").type == 0)
				{
					context.OnInputEvent("Keyboard_n", event);
					firedKeys.push_back("Keyboard_n");
				}
				break;
			}
			case SDLK_o:				
			{
				if (context.GetKeyBinding("Keyboard_o") != "" && context.GetKeyBoundEvent("Keyboard_o").type == 0)
				{
					context.OnInputEvent("Keyboard_o", event);
					firedKeys.push_back("Keyboard_o");
				}
				break;
			}
			case SDLK_p:				
			{
				if (context.GetKeyBinding("Keyboard_p") != "" && context.GetKeyBoundEvent("Keyboard_p").type == 0)
				{
					context.OnInputEvent("Keyboard_p", event);
					firedKeys.push_back("Keyboard_p");
				}
				break;
			}
			case SDLK_q:				
			{
				if (context.GetKeyBinding("Keyboard_q") != "" && context.GetKeyBoundEvent("Keyboard_q").type == 0)
				{
					context.OnInputEvent("Keyboard_q", event);
					firedKeys.push_back("Keyboard_q");
				}
				break;
			}
			case SDLK_r:			
			{
				if (context.GetKeyBinding("Keyboard_r") != "" && context.GetKeyBoundEvent("Keyboard_r").type == 0)
				{
					context.OnInputEvent("Keyboard_r", event);
					firedKeys.push_back("Keyboard_r");
				}
				break;
			}
			case SDLK_s:				
			{
				if (context.GetKeyBinding("Keyboard_s") != "" && context.GetKeyBoundEvent("Keyboard_s").type == 0)
				{
					context.OnInputEvent("Keyboard_s", event);
					firedKeys.push_back("Keyboard_s");
				}
				break;
			}
			case SDLK_t:				
			{
				if (context.GetKeyBinding("Keyboard_t") != "" && context.GetKeyBoundEvent("Keyboard_t").type == 0)
				{
					context.OnInputEvent("Keyboard_t", event);
					firedKeys.push_back("Keyboard_t");
				}
				break;
			}
			case SDLK_u:				
			{
				if (context.GetKeyBinding("Keyboard_u") != "" && context.GetKeyBoundEvent("Keyboard_u").type == 0)
				{
					context.OnInputEvent("Keyboard_u", event);
					firedKeys.push_back("Keyboard_u");
				}
				break;
			}
			case SDLK_v:				
			{
				if (context.GetKeyBinding("Keyboard_v") != "" && context.GetKeyBoundEvent("Keyboard_v").type == 0)
				{
					context.OnInputEvent("Keyboard_v", event);
					firedKeys.push_back("Keyboard_v");
				}
				break;
			}
			case SDLK_w:
			{
				if (context.GetKeyBinding("Keyboard_w") != "" && context.GetKeyBoundEvent("Keyboard_w").type == 0)
				{
					context.OnInputEvent("Keyboard_w", event);
					firedKeys.push_back("Keyboard_w");
				}
				break;
			}
			case SDLK_x:				
			{
				if (context.GetKeyBinding("Keyboard_x") != "" && context.GetKeyBoundEvent("Keyboard_x").type == 0)
				{
					context.OnInputEvent("Keyboard_x", event);
					firedKeys.push_back("Keyboard_x");
				}
				break;
			}
			case SDLK_y:				
			{
				if (context.GetKeyBinding("Keyboard_y") != "" && context.GetKeyBoundEvent("Keyboard_y").type == 0)
				{
					context.OnInputEvent("Keyboard_y", event);
					firedKeys.push_back("Keyboard_y");
				}
				break;
			}
			case SDLK_z:				
			{
				if (context.GetKeyBinding("Keyboard_z") != "" && context.GetKeyBoundEvent("Keyboard_z").type == 0)
				{
					context.OnInputEvent("Keyboard_z", event);
					firedKeys.push_back("Keyboard_z");
				}
				break;
			}
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
				if (context.GetKeyBinding("Keyboard_space") != "")
					context.ClearInputActionEvent("Keyboard_space");
				break;

			case SDLK_UP:				
				if (context.GetKeyBinding("Keyboard_up") != "")
					context.ClearInputActionEvent("Keyboard_up");
				break;

			case SDLK_DOWN:				
				if (context.GetKeyBinding("Keyboard_down") != "")
					context.ClearInputActionEvent("Keyboard_down");
				break;

			case SDLK_LEFT:				
				if (context.GetKeyBinding("Keyboard_left") != "")
					context.ClearInputActionEvent("Keyboard_left");
				break;

			case SDLK_RIGHT:				
				if (context.GetKeyBinding("Keyboard_right") != "")
					context.ClearInputActionEvent("Keyboard_right");
				break;

			case SDLK_a:				
				if (context.GetKeyBinding("Keyboard_a") != "")
					context.ClearInputActionEvent("Keyboard_a");
				break;

			case SDLK_b:				
				if (context.GetKeyBinding("Keyboard_b") != "")
					context.ClearInputActionEvent("Keyboard_b");
				break;

			case SDLK_c:
				if (context.GetKeyBinding("Keyboard_c") != "")
					context.ClearInputActionEvent("Keyboard_c");
				break;

			case SDLK_d:				
				if (context.GetKeyBinding("Keyboard_d") != "")
					context.ClearInputActionEvent("Keyboard_d");
				break;

			case SDLK_e:				
				if (context.GetKeyBinding("Keyboard_e") != "")
					context.ClearInputActionEvent("Keyboard_e");
				break;

			case SDLK_f:				
				if (context.GetKeyBinding("Keyboard_f") != "")
					context.ClearInputActionEvent("Keyboard_f");
				break;

			case SDLK_g:				
				if (context.GetKeyBinding("Keyboard_g") != "")
					context.ClearInputActionEvent("Keyboard_g");
				break;

			case SDLK_h:				
				if (context.GetKeyBinding("Keyboard_h") != "")
					context.ClearInputActionEvent("Keyboard_h");
				break;

			case SDLK_i:				
				if (context.GetKeyBinding("Keyboard_i") != "")
					context.ClearInputActionEvent("Keyboard_i");
				break;

			case SDLK_j:				
				if (context.GetKeyBinding("Keyboard_j") != "")
					context.ClearInputActionEvent("Keyboard_j");
				break;

			case SDLK_k:				
				if (context.GetKeyBinding("Keyboard_k") != "")
					context.ClearInputActionEvent("Keyboard_k");
				break;

			case SDLK_l:				
				if (context.GetKeyBinding("Keyboard_l") != "")
					context.ClearInputActionEvent("Keyboard_l");
				break;

			case SDLK_m:				
				if (context.GetKeyBinding("Keyboard_m") != "")
					context.ClearInputActionEvent("Keyboard_m");
				break;

			case SDLK_n:				
				if (context.GetKeyBinding("Keyboard_n") != "")
					context.ClearInputActionEvent("Keyboard_n");
				break;

			case SDLK_o:				
				if (context.GetKeyBinding("Keyboard_o") != "")
					context.ClearInputActionEvent("Keyboard_o");
				break;

			case SDLK_p:
				if (context.GetKeyBinding("Keyboard_p") != "")
					context.ClearInputActionEvent("Keyboard_p");
				break;

			case SDLK_q:				
				if (context.GetKeyBinding("Keyboard_q") != "")
					context.ClearInputActionEvent("Keyboard_q");
				break;

			case SDLK_r:				
				if (context.GetKeyBinding("Keyboard_r") != "")
					context.ClearInputActionEvent("Keyboard_r");
				break;

			case SDLK_s:				
				if (context.GetKeyBinding("Keyboard_s") != "")
					context.ClearInputActionEvent("Keyboard_s");
				break;

			case SDLK_t:				
				if (context.GetKeyBinding("Keyboard_t") != "")
					context.ClearInputActionEvent("Keyboard_t");
				break;

			case SDLK_u:				
				if (context.GetKeyBinding("Keyboard_u") != "")
					context.ClearInputActionEvent("Keyboard_u");
				break;

			case SDLK_v:				
				if (context.GetKeyBinding("Keyboard_v") != "")
					context.ClearInputActionEvent("Keyboard_v");
				break;

			case SDLK_w:				
				if (context.GetKeyBinding("Keyboard_w") != "")
					context.ClearInputActionEvent("Keyboard_w");
				break;

			case SDLK_x:			
				if (context.GetKeyBinding("Keyboard_x") != "")
					context.ClearInputActionEvent("Keyboard_x");
				break;

			case SDLK_y:			
				if (context.GetKeyBinding("Keyboard_y") != "")
					context.ClearInputActionEvent("Keyboard_y");
				break;

			case SDLK_z:				
				if (context.GetKeyBinding("Keyboard_z") != "")
					context.ClearInputActionEvent("Keyboard_z");
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
				if (event.jaxis.value > -F_JOYSTICK_DEAD_ZONE && event.jaxis.value < F_JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;				
				if (context.GetKeyBinding("XInput_LeftJoystickX") != "")
					context.OnInputEvent("XInput_LeftJoystickX", event);
				break;
			case XInputAxis::LeftYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -F_JOYSTICK_DEAD_ZONE && event.jaxis.value < F_JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;				
				if (context.GetKeyBinding("XInput_LeftJoystickY") != "")
					context.OnInputEvent("XInput_LeftJoystickY", event);
				break;
			case XInputAxis::RightXAxis:
				// Left of dead zone or Right of dead zone
				if (event.jaxis.value > -F_JOYSTICK_DEAD_ZONE && event.jaxis.value < F_JOYSTICK_DEAD_ZONE)
					event.jaxis.value = 0;			
				if (context.GetKeyBinding("XInput_RightJoystick") != "")
					context.OnInputEvent("XInput_RightJoystick", event);
				break;
			case XInputAxis::RightYAxis:
				// Below dead zone or Above dead zone
				if (event.jaxis.value > -F_JOYSTICK_DEAD_ZONE && event.jaxis.value < F_JOYSTICK_DEAD_ZONE)
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
			case XInputHats::Hat_Up:
				//LogFloat(event.jhat.type, "Hat Type: ");
				//LogFloat(event.jhat.value, "Hat Value: ");
				//LogFloat(event.jhat.hat, "Hat hat: ");
				break;
			case XInputHats::Hat_Down:
				FlatEngine::LogString("Down");
				break;
			case XInputHats::Hat_Left:
				FlatEngine::LogString("Left");
				break;
			case XInputHats::Hat_Right:
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
		{
			filePath = GetDir("tileSets") + "\\" + fileName + ".tls";
		}
		else
		{
			filePath = path + "\\" + fileName + ".tls";
		}

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
		SDL_RenderSetScale(F_Window->GetRenderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(F_Window->GetRenderer(), (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(F_Window->GetRenderer());
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

		// TO DO 
		// When vsync is off, find a way to only call renderpresent once per refresh depending on the rate of the screen
		//Uint32 renderPresentStart = FlatEngine::GetEngineTime(); // Profiler
		SDL_RenderPresent(F_Window->GetRenderer());
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
		std::string filePath = "";

		if (path == "")
			filePath = GetDir("animations") + filename + ".anm";
		else
			filePath = path + "\\" + filename + ".anm";

		std::shared_ptr<Animation::S_AnimationProperties> propertiesObject = std::make_shared< Animation::S_AnimationProperties>();
		propertiesObject->animationName = filename;
		propertiesObject->animationPath = filePath;
		SaveAnimationFile(propertiesObject, filePath);
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


		// Event
		json eventProps = json::array();
		for (std::shared_ptr<Animation::S_Event> eventProp : propertiesObject->eventProps)
		{
			json jsonData = {
				{ "functionName", eventProp->functionName },
				{ "time", eventProp->time },
			};
			std::string data = jsonData.dump();
			eventProps.push_back(json::parse(data));
		}
		
		// Transform
		json transformProps = json::array();
		for (std::shared_ptr<Animation::S_Transform> transformProp : propertiesObject->transformProps)
		{		
			json jsonData = {
				{ "transformInterpType", transformProp->transformInterpType },
				{ "transformSpeed", transformProp->transformSpeed },
				{ "scaleInterpType", transformProp->scaleInterpType },
				{ "scaleSpeed", transformProp->scaleSpeed },
				{ "time", transformProp->time },
				{ "xPos", transformProp->xPos },
				{ "yPos", transformProp->yPos },
				{ "xScale", transformProp->xScale },
				{ "yScale", transformProp->yScale },
				{ "_posAnimated", transformProp->b_posAnimated },
				{ "_scaleAnimated", transformProp->b_scaleAnimated }
			};
			std::string data = jsonData.dump();
			transformProps.push_back(json::parse(data));
		}

		// Sprite
		json spriteProps = json::array();
		for (std::shared_ptr<Animation::S_Sprite> spriteProp : propertiesObject->spriteProps)
		{
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
				{ "_instantTintChange", spriteProp->b_instantTintChange },
				{ "_pathAnimated", spriteProp->b_pathAnimated },
				{ "_offsetAnimated", spriteProp->b_offsetAnimated },
				{ "_scaleAnimated", spriteProp->b_scaleAnimated },
				{ "_tintColorAnimated", spriteProp->b_tintColorAnimated }
			};

			std::string data = jsonData.dump();
			spriteProps.push_back(json::parse(data));
		}

		// Camera
		json cameraProps = json::array();
		for (std::shared_ptr<Animation::S_Camera> cameraProp : propertiesObject->cameraProps)
		{
			json jsonData = {
				{ "time", cameraProp->time },
				{ "_isPrimaryCamera", cameraProp->b_isPrimaryCamera }
			};
			std::string data = jsonData.dump();
			cameraProps.push_back(json::parse(data));
		}

		// Script
		json scriptProps = json::array();
		for (std::shared_ptr<Animation::S_Script> scriptProp : propertiesObject->scriptProps)
		{
			json jsonData = {
				{ "time", scriptProp->time },
				{ "path", scriptProp->path }
			};
			std::string data = jsonData.dump();
			scriptProps.push_back(json::parse(data));
		}

		// Button
		json buttonProps = json::array();
		for (std::shared_ptr<Animation::S_Button> buttonProp : propertiesObject->buttonProps)
		{
			json jsonData = {
				{ "time", buttonProp->time },
				{ "_isActive", buttonProp->b_isActive }
			};
			std::string data = jsonData.dump();
			buttonProps.push_back(json::parse(data));
		}

		// Canvas
		json canvasProps = json::array();
		for (std::shared_ptr<Animation::S_Canvas> canvasProp : propertiesObject->canvasProps)
		{
			json jsonData = {
				{ "time", canvasProp->time }
			};
			std::string data = jsonData.dump();
			canvasProps.push_back(json::parse(data));
		}

		// Audio
		json audioProps = json::array();
		for (std::shared_ptr<Animation::S_Audio> audioProp : propertiesObject->audioProps)
		{
			json jsonData = {
				{ "time", audioProp->time },
				{ "soundName", audioProp->soundName },
				{ "_stopAllOtherSounds", audioProp->b_stopAllOtherSounds }
			};
			std::string data = jsonData.dump();
			audioProps.push_back(json::parse(data));
		}

		// Text
		json textProps = json::array();
		for (std::shared_ptr<Animation::S_Text> textProp : propertiesObject->textProps)
		{
			json jsonData = {
				{ "time", textProp->time },
				{ "fontPath", textProp->fontPath },
				{ "text", textProp->text },
				{ "xOffset", textProp->xOffset },
				{ "yOffset", textProp->yOffset },
				{ "tintColorX", textProp->tintColor.x },
				{ "tintColorY", textProp->tintColor.y },
				{ "tintColorZ", textProp->tintColor.z },
				{ "tintColorW", textProp->tintColor.w },
				{ "_instantTintChange", textProp->b_instantTintChange },
				{ "_fontPathAnimated", textProp->b_fontPathAnimated },
				{ "_textAnimated", textProp->b_textAnimated },
				{ "_tintColorAnimated", textProp->b_tintColorAnimated },
				{ "_offsetAnimated", textProp->b_offsetAnimated }
			};
			std::string data = jsonData.dump();
			textProps.push_back(json::parse(data));
		}

		// BoxCollider
		json boxColliderProps = json::array();
		for (std::shared_ptr<Animation::S_BoxCollider> boxColliderProp : propertiesObject->boxColliderProps)
		{
			json jsonData = {
				{ "time", boxColliderProp->time },
				{ "_isActive", boxColliderProp->b_isActive }
			};
			std::string data = jsonData.dump();
			boxColliderProps.push_back(json::parse(data));
		}

		// CircleCollider
		json circleColliderProps = json::array();
		for (std::shared_ptr<Animation::S_CircleCollider> circleColliderProp : propertiesObject->circleColliderProps)
		{
			json jsonData = {
				{ "time", circleColliderProp->time },
				{ "_isActive", circleColliderProp->b_isActive }
			};
			std::string data = jsonData.dump();
			circleColliderProps.push_back(json::parse(data));
		}

		// RigidBody
		json rigidBodyProps = json::array();
		for (std::shared_ptr<Animation::S_RigidBody> rigidBodyProp : propertiesObject->rigidBodyProps)
		{
			json jsonData = {
				{ "time", rigidBodyProp->time },
				{ "interpType", rigidBodyProp->interpType },
				{ "speed", rigidBodyProp->speed },
				{ "_isActive", rigidBodyProp->b_isActive },
				{ "gravityScale", rigidBodyProp->gravityScale },
			};
			std::string data = jsonData.dump();
			rigidBodyProps.push_back(json::parse(data));
		}

		// CharacterController
		json characterControllerProps = json::array();
		for (std::shared_ptr<Animation::S_CharacterController> characterControllerProp : propertiesObject->characterControllerProps)
		{
			json jsonData = {
				{ "time", characterControllerProp->time },
				{ "_isActive", characterControllerProp->b_isActive }
			};
			std::string data = jsonData.dump();
			characterControllerProps.push_back(json::parse(data));
		}

		json animProps = json::object({
			{ "event", eventProps },
			{ "transform", transformProps },
			{ "sprite", spriteProps },
			{ "camera", cameraProps },
			{ "script", scriptProps },
			{ "button", buttonProps },
			{ "canvas", canvasProps },
			{ "audio", audioProps },
			{ "text", textProps },
			{ "boxCollider", boxColliderProps },
			{ "circleCollider", circleColliderProps },
			{ "rigidBody", rigidBodyProps },
			{ "characterController", characterControllerProps }
		});

		json animationData = json::object({
			{ "name", propertiesObject->animationName },
			{ "length", propertiesObject->animationLength },
			{ "_loop", propertiesObject->b_loop },
			{ "animationProperties", animProps }
		});

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"animation", animationData } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path)
	{
		std::shared_ptr<Animation::S_AnimationProperties> animProps = std::make_shared<Animation::S_AnimationProperties>();
		std::shared_ptr<Animation::S_Event> eventProperties;
		std::shared_ptr<Animation::S_Transform> transformProperties;
		std::shared_ptr<Animation::S_Sprite > spriteProperties;
		animProps->animationPath = path;

		std::ofstream file_obj;
		std::ifstream ifstream(path);
		file_obj.open(path, std::ios::in);

		std::string fileContent = "";

		if (file_obj.good())
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		file_obj.close();

		if (file_obj.good())
		{
			json fileContentJson = json::parse(fileContent);

			if (fileContentJson.contains("animation"))
			{
				animProps->animationName = "New Animation";
				json animationJson = fileContentJson["animation"];
				std::string animName = CheckJsonString(animationJson, "name", "name");

				animProps->animationName = animName;
				animProps->animationLength = CheckJsonFloat(animationJson, "length", animName);
				animProps->b_loop = CheckJsonBool(animationJson, "_loop", animName);

				// Event
				json eventProps = animationJson["animationProperties"]["event"];
				for (int i = 0; i < eventProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Event> frame = std::make_shared<Animation::S_Event>();
					frame->name = "Event";
					frame->functionName = CheckJsonString(eventProps[i], "functionName", animName);
					frame->time = CheckJsonFloat(eventProps[i], "time", animName);
					animProps->eventProps.push_back(frame);
				}

				// Transform
				json transformProps = animationJson["animationProperties"]["transform"];
				for (int i = 0; i < transformProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Transform> frame = std::make_shared<Animation::S_Transform>();
					frame->name = "Transform";
					frame->transformInterpType = (Animation::InterpType)CheckJsonInt(transformProps[i], "transformInterpType", animName);
					frame->scaleInterpType = (Animation::InterpType)CheckJsonInt(transformProps[i], "scaleInterpType", animName);
					frame->scaleSpeed = CheckJsonFloat(transformProps[i], "scaleSpeed", animName);
					frame->time = CheckJsonFloat(transformProps[i], "time", animName);
					frame->xPos = CheckJsonFloat(transformProps[i], "xPos", animName);
					frame->yPos = CheckJsonFloat(transformProps[i], "yPos", animName);
					frame->xScale = CheckJsonFloat(transformProps[i], "xScale", animName);
					frame->yScale = CheckJsonFloat(transformProps[i], "yScale", animName);
					frame->b_posAnimated = CheckJsonBool(transformProps[i], "_posAnimated", animName);
					frame->b_scaleAnimated = CheckJsonBool(transformProps[i], "_scaleAnimated", animName);
					animProps->transformProps.push_back(frame);
				}

				// Sprite
				json spriteProps = animationJson["animationProperties"]["sprite"];
				for (int i = 0; i < spriteProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Sprite> frame = std::make_shared<Animation::S_Sprite>();
					frame->name = "Sprite";
					frame->interpType = (Animation::InterpType)CheckJsonInt(spriteProps[i], "interpType", animName);
					frame->speed = CheckJsonFloat(spriteProps[i], "speed", animName);
					frame->time = CheckJsonFloat(spriteProps[i], "time", animName);
					frame->xOffset = CheckJsonFloat(spriteProps[i], "xOffset", animName);
					frame->yOffset = CheckJsonFloat(spriteProps[i], "yOffset", animName);
					frame->path = CheckJsonString(spriteProps[i], "path", animName);
					frame->b_instantTintChange = CheckJsonBool(spriteProps[i], "_instantTintChange", animName);
					frame->tintColor = Vector4(
						CheckJsonFloat(spriteProps[i], "tintColorX", animName),
						CheckJsonFloat(spriteProps[i], "tintColorY", animName),
						CheckJsonFloat(spriteProps[i], "tintColorZ", animName),
						CheckJsonFloat(spriteProps[i], "tintColorW", animName)
					);
					frame->b_pathAnimated = CheckJsonBool(spriteProps[i], "_pathAnimated", animName);
					frame->b_scaleAnimated = CheckJsonBool(spriteProps[i], "_scaleAnimated", animName);
					frame->b_offsetAnimated = CheckJsonBool(spriteProps[i], "_offsetAnimated", animName);
					frame->b_tintColorAnimated = CheckJsonBool(spriteProps[i], "_tintColorAnimated", animName);
					animProps->spriteProps.push_back(frame);			
				}

				// Camera
				json cameraProps = animationJson["animationProperties"]["camera"];
				for (int i = 0; i < cameraProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Camera> frame = std::make_shared<Animation::S_Camera>();
					frame->name = "Camera";
					frame->time = CheckJsonFloat(cameraProps[i], "time", animName);
					frame->b_isPrimaryCamera = CheckJsonBool(cameraProps[i], "_isPrimaryCamera", animName);
					animProps->cameraProps.push_back(frame);
				}

				// Script
				json scriptProps = animationJson["animationProperties"]["script"];
				for (int i = 0; i < scriptProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Script> frame = std::make_shared<Animation::S_Script>();
					frame->name = "Script";
					frame->time = CheckJsonFloat(scriptProps[i], "time", animName);
					frame->path = CheckJsonString(scriptProps[i], "path", animName);
					animProps->scriptProps.push_back(frame);
				}

				// Button
				json buttonProps = animationJson["animationProperties"]["button"];
				for (int i = 0; i < buttonProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Button> frame = std::make_shared<Animation::S_Button>();
					frame->name = "Button";
					frame->time = CheckJsonFloat(buttonProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(buttonProps[i], "_isActive", animName);
					animProps->buttonProps.push_back(frame);
				}

				// Canvas
				json canvasProps = animationJson["animationProperties"]["canvas"];
				for (int i = 0; i < canvasProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Canvas> frame = std::make_shared<Animation::S_Canvas>();
					frame->name = "Canvas";
					frame->time = CheckJsonFloat(canvasProps[i], "time", animName);
					animProps->canvasProps.push_back(frame);
				}

				// Audio
				json audioProps = animationJson["animationProperties"]["audio"];
				for (int i = 0; i < audioProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Audio> frame = std::make_shared<Animation::S_Audio>();
					frame->name = "Audio";
					frame->time = CheckJsonFloat(audioProps[i], "time", animName);
					frame->soundName = CheckJsonString(audioProps[i], "soundName", animName);
					frame->b_stopAllOtherSounds = CheckJsonBool(audioProps[i], "_stopAllOtherSounds", animName);
					animProps->audioProps.push_back(frame);
				}

				// Text
				json textProps = animationJson["animationProperties"]["text"];
				for (int i = 0; i < textProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Text> frame = std::make_shared<Animation::S_Text>();
					frame->name = "Text";
					frame->time = CheckJsonFloat(textProps[i], "time", animName);
					frame->fontPath = CheckJsonString(textProps[i], "fontPath", frame->name);
					frame->text = CheckJsonString(textProps[i], "text", frame->name);
					frame->tintColor = Vector4(
						CheckJsonFloat(textProps[i], "tintColorX", frame->name),
						CheckJsonFloat(textProps[i], "tintColorY", frame->name),
						CheckJsonFloat(textProps[i], "tintColorZ", frame->name),
						CheckJsonFloat(textProps[i], "tintColorW", frame->name)
					);
					frame->b_fontPathAnimated = CheckJsonBool(textProps[i], "_fontPathAnimated", animName);
					frame->b_textAnimated = CheckJsonBool(textProps[i], "_textAnimated", animName);
					frame->b_tintColorAnimated = CheckJsonBool(textProps[i], "_tintColorAnimated", animName);
					frame->b_offsetAnimated = CheckJsonBool(textProps[i], "_offsetAnimated", animName);
					animProps->textProps.push_back(frame);
				}

				// BoxCollider
				json boxColliderProps = animationJson["animationProperties"]["boxCollider"];
				for (int i = 0; i < boxColliderProps.size(); i++)
				{
					std::shared_ptr<Animation::S_BoxCollider> frame = std::make_shared<Animation::S_BoxCollider>();
					frame->name = "BoxCollider";
					frame->time = CheckJsonFloat(boxColliderProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(boxColliderProps[i], "_isActive", animName);
					animProps->boxColliderProps.push_back(frame);
				}

				// CircleCollider
				json circleColliderProps = animationJson["animationProperties"]["circleCollider"];
				for (int i = 0; i < circleColliderProps.size(); i++)
				{
					std::shared_ptr<Animation::S_CircleCollider> frame = std::make_shared<Animation::S_CircleCollider>();
					frame->name = "CircleCollider";
					frame->time = CheckJsonFloat(circleColliderProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(circleColliderProps[i], "_isActive", animName);
					animProps->circleColliderProps.push_back(frame);
				}

				// RigidBody
				json rigidBodyProps = animationJson["animationProperties"]["rigidBody"];
				for (int i = 0; i < rigidBodyProps.size(); i++)
				{
					std::shared_ptr<Animation::S_RigidBody> frame = std::make_shared<Animation::S_RigidBody>();
					frame->name = "RigidBody";
					frame->time = CheckJsonFloat(rigidBodyProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(rigidBodyProps[i], "_isActive", animName);
					frame->interpType = (Animation::InterpType)CheckJsonInt(rigidBodyProps[i], "interpType", animName);
					frame->speed = CheckJsonFloat(rigidBodyProps[i], "speed", animName);
					frame->gravityScale = CheckJsonFloat(rigidBodyProps[i], "gravityScale", animName);
					animProps->rigidBodyProps.push_back(frame);
				}

				// CharacterController
				json characterControllerProps = animationJson["animationProperties"]["characterController"];
				for (int i = 0; i < characterControllerProps.size(); i++)
				{
					std::shared_ptr<Animation::S_CharacterController> frame = std::make_shared<Animation::S_CharacterController>();
					frame->name = "CharacterController";
					frame->time = CheckJsonFloat(characterControllerProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(characterControllerProps[i], "_isActive", animName);
					animProps->characterControllerProps.push_back(frame);
				}
			}
		}

		return animProps;
	}


	// Prefabs
	void CreatePrefab(std::string path, GameObject &gameObject)
	{
		F_PrefabManager->CreatePrefab(path, gameObject);
	}

	void InitializePrefabs()
	{
		F_PrefabManager->InitializePrefabs();
	}

	GameObject *Instantiate(std::string prefabName, Vector2 position, long parentID, long ID)
	{
		return F_PrefabManager->Instantiate(prefabName, position, parentID, ID);
	}

	//std::map<std::string, Prefab> GetPrefabs()
	//{
	//	return F_PrefabManager->GetPrefabs();
	//}


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

	long GetFramesCounted()
	{
		return F_Application->GetGameLoop()->GetFramesCounted();
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

	std::string GetFilePathUsingFileName(std::string dirPath, std::string name)
	{
		std::string file = "";

		for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
		{
			if (p.path().stem().string() == name || p.path().string().find(name) != std::string::npos)
			{
				file = p.path().string();
			}
		}

		return file;
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
			FlatEngine::LogError("JsonContains() - \"" + loadedName + "\" does not contain a value for \"" + checkFor + "\".");
		return contains;
	}

	float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName)
	{
		float value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogError("CheckJsonFloat() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		return value;
	}

	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
	{
		int value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogError("CheckJsonInt() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		return value;
	}

	long CheckJsonLong(json obj, std::string checkFor, std::string loadedName)
	{
		long value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogError("CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		return value;
	}

	bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName)
	{
		bool value = false;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogError("CheckJsonBool() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		return value;
	}

	std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName)
	{
		std::string value = "Failed to load.";
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogError("CheckJsonString() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		return value;
	}

	GameObject *CreateObjectFromJson(json objectJson)
	{
		GameObject *loadedObject;
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

		if (_isPrefab)
		{
			loadedObject = Instantiate(prefabName, spawnLocation, loadedParentID, loadedID);
			loadedObject->SetName(objectName);
		}
		else
		{
			loadedObject = CreateGameObject(loadedParentID, loadedID);
			loadedObject->SetName(objectName);
			loadedObject->SetActive(_isActive);

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
			loadedObject->SetTagList(tags);

			float objectRotation = 0;

			// Loop through the components in this GameObjects json
			for (int j = 0; j < objectJson["components"].size(); j++)
			{
				json componentJson = objectJson["components"][j];
				std::string type = CheckJsonString(componentJson, "type", objectName);
				long id = CheckJsonLong(componentJson, "id", objectName);
				bool _isCollapsed = CheckJsonBool(componentJson, "_isCollapsed", objectName);
				bool _isActive = CheckJsonBool(componentJson, "_isActive", objectName);

				if (type == "Transform")
				{
					Transform* transform = loadedObject->GetTransform();
					float rotation = CheckJsonFloat(componentJson, "rotation", objectName);
					transform->SetID(id);
					transform->SetActive(_isActive);
					transform->SetCollapsed(_isCollapsed);
					transform->SetOrigin(Vector2(CheckJsonFloat(componentJson, "xOrigin", objectName), CheckJsonFloat(componentJson, "yOrigin", objectName)));
					transform->SetInitialPosition(Vector2(CheckJsonFloat(componentJson, "xPos", objectName), CheckJsonFloat(componentJson, "yPos", objectName)));
					transform->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					transform->SetRotation(rotation);
					objectRotation = rotation;
				}
				else if (type == "Sprite")
				{
					Sprite* newSprite = loadedObject->AddSprite(id, _isActive, _isCollapsed);
					std::string pivotPoint = "Center";
					newSprite->SetPivotPoint(CheckJsonString(componentJson, "pivotPoint", objectName));					
					newSprite->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));				
					newSprite->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newSprite->SetTintColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
					newSprite->SetTexture(CheckJsonString(componentJson, "path", objectName));
					newSprite->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
				}
				else if (type == "Camera")
				{
					Camera* newCamera = loadedObject->AddCamera(id, _isActive, _isCollapsed);
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
					newCamera->SetToFollowID(CheckJsonLong(componentJson, "following", objectName));
				}
				else if (type == "Script")
				{
					Script* newScript = loadedObject->AddScript(id, _isActive, _isCollapsed);
					newScript->SetAttachedScript(CheckJsonString(componentJson, "attachedScript", objectName));
				}
				else if (type == "Button")
				{
					Button* newButton = loadedObject->AddButton(id, _isActive, _isCollapsed);
					newButton->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newButton->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newButton->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
				}
				else if (type == "Canvas")
				{
					Canvas* newCanvas = loadedObject->AddCanvas(id, _isActive, _isCollapsed);
					newCanvas->SetDimensions(CheckJsonFloat(componentJson, "width", objectName), CheckJsonFloat(componentJson, "height", objectName));
					newCanvas->SetLayerNumber(CheckJsonInt(componentJson, "layerNumber", objectName));
					newCanvas->SetBlocksLayers(CheckJsonBool(componentJson, "_blocksLayers", objectName));
				}
				else if (type == "Animation")
				{
					Animation* newAnimation = loadedObject->AddAnimation(id, _isActive, _isCollapsed);
					newAnimation->SetAnimationPath(CheckJsonString(componentJson, "path", objectName));
				}
				else if (type == "Audio")
				{
					Audio* newAudio = loadedObject->AddAudio(id, _isActive, _isCollapsed);				

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
					Text* newText = loadedObject->AddText(id, _isActive, _isCollapsed);
					newText->SetFontPath(CheckJsonString(componentJson, "fontPath", objectName));
					newText->SetFontSize(CheckJsonInt(componentJson, "fontSize", objectName));
					newText->SetPivotPoint(CheckJsonString(componentJson, "pivotPoint", objectName));
					newText->SetColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
					newText->SetText(CheckJsonString(componentJson, "text", objectName));
					newText->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newText->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
				}
				else if (type == "CharacterController")
				{
					CharacterController* newCharacterController = loadedObject->AddCharacterController(id, _isActive, _isCollapsed);
					newCharacterController->SetMaxAcceleration(CheckJsonFloat(componentJson, "maxAcceleration", objectName));
					newCharacterController->SetMaxSpeed(CheckJsonFloat(componentJson, "maxSpeed", objectName));
					newCharacterController->SetAirControl(CheckJsonFloat(componentJson, "airControl", objectName));
				}
				else if (type == "BoxCollider")
				{
					BoxCollider* newBoxCollider = loadedObject->AddBoxCollider(id, _isActive, _isCollapsed);
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
					CircleCollider* newCircleCollider = loadedObject->AddCircleCollider(id, _isActive, _isCollapsed);
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
					RigidBody* newRigidBody = loadedObject->AddRigidBody(id, _isActive, _isCollapsed);
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
					TileMap* newTileMap = loadedObject->AddTileMap(id, _isActive, _isCollapsed);
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
							std::string tileSetName = CheckJsonString(tileSetJson, "name", objectName);
							if (GetTileSet(tileSetName) != nullptr)
							{
								newTileMap->AddTileSet(tileSetName);
							}
							else
							{
								LogError("TileSet: \"" + tileSetName + "\" could not be found.");
							}
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
				
							if (tileSetName != "" && GetTileSet(tileSetName) != nullptr && tileSetIndex != -1)
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
		if (loadedObject != nullptr)
		{
			if (loadedObject->GetRigidBody() != nullptr)
			{
				loadedObject->GetRigidBody()->UpdateI();
			}

			if (loadedObject->GetButton() != nullptr)
			{
				loadedObject->GetButton()->CalculateActiveEdges();
			}
		}

		return loadedObject;
	}
}

// ImGui cheat sheet
// Border around object
//auto wPos = ImGui::GetWindowPos();
//auto wSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
//ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, GetColor32("componentBorder"), 2);

