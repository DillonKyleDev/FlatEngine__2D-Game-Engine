#include "FlatEngine.h"
#include "UIManager.h"
//#include "../GameManager.h"
#include "Animation.h"
#include "GameLoop.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "BoxCollider.h"
#include "ScriptComponent.h"
#include "Project.h"
#include "Sound.h"
#include "MappingContext.h"
#include "TextureManager.h"
#include "PrefabManager.h"
#include "Logger.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "Window.h"
#include "ECSManager.h"

#include <SDL.h>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <shobjidl.h> 


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

	// Managers
	TextureManager F_TextureManager = TextureManager();
	Logger F_Logger = Logger();
	UIManager F_UIManager = UIManager();
	SceneManager F_SceneManager = SceneManager();
	//GameManager F_GameManager = GameManager();
	Sound F_SoundController = Sound();
	ECSManager F_ECSManager = ECSManager();

	std::vector<std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>> colliderPairs = std::vector<std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>>();
	std::shared_ptr<PrefabManager> prefabManager = std::make_shared<PrefabManager>();
	GameObject* playerObject = nullptr;

	// Mapping Context
	std::vector<std::shared_ptr<MappingContext>> mappingContexts;

	// Animator
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = std::make_shared<Animation::S_AnimationProperties>();
	std::shared_ptr<GameObject> objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	int previewAnimationStartTime = 0;
	int previewAnimationTime = 0;
	bool _playPreviewAnimation = true;


	bool Init()
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
			if (Window::Init(title, 1920, 1000))
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
							Mix_AllocateChannels(100);
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

	void CloseProgram()
	{
		// Clean up old gamepads
		for (SDL_Joystick* gamepad : gamepads)
		{
			SDL_JoystickClose(gamepad);
			gamepad = NULL;
		}

		//Quit TextureManager
		FlatEngine::F_TextureManager.Cleanup();

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


	// GameObject / Scene management
	void SaveScene(std::shared_ptr<Scene> scene, std::string filename)
	{
		F_SceneManager.SaveScene(scene, filename);
	}

	void LoadScene(std::string name)
	{
		// Stop any playing music
		F_SoundController.StopMusic();
		// Reset buttons in UIManager
		F_UIManager.ResetButtons();
		if (F_SceneManager.GetLoadedScene() != nullptr)
			F_SceneManager.SaveAnimationPreviewObjects();
		F_SceneManager.LoadScene(name);
		F_SceneManager.LoadAnimationPreviewObjects();

		// If the GameLoop is running, reinitialize the new scene's GameObjects
		if (FlatEngine::GameLoopStarted())
			FlatEngine::F_Application->GetGameLoop()->InitializeScriptObjects(GetSceneObjects());

		if (GetObjectByName("Player") != nullptr)
			playerObject = GetObjectByName("Player");
	}

	long GetNextComponentID()
	{
		long nextID = -1;
		std::shared_ptr<Scene> loadedScene = GetLoadedScene();
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
		std::shared_ptr<Scene> loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextGameObjectID();
		}

		return nextID;
	}

	std::shared_ptr<Scene> GetLoadedScene()
	{
		return F_SceneManager.GetLoadedScene();
	}

	std::shared_ptr<Scene> CreateNewScene()
	{
		return F_SceneManager.CreateNewScene();
	}

	std::string GetLoadedScenePath()
	{
		return F_SceneManager.GetLoadedScenePath();
	}

	std::vector<GameObject> &GetSceneObjects()
	{
		//if (GetLoadedScene() != nullptr)
			return GetLoadedScene()->GetSceneObjects();
		//else
		//	return std::vector<GameObject>();
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


	// Mapping Context Management
	void SaveMappingContext(std::string path, std::shared_ptr<MappingContext> context)
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

		if (context->GetKeyBindings().size() > 0)
		{
			std::string data = context->GetData();
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
		mappingContexts.clear();

		std::string path = "C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\WindowsApplication\\mappingContext";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			// Create a new context to save the loaded keybindings to
			std::shared_ptr<MappingContext> newContext = std::make_shared<MappingContext>();

			json contextData = LoadFileData(entry.path().string());
			if (contextData != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto mappings = contextData["Mapping Context"][0];
				
				newContext->SetName(mappings["name"]);
				newContext->SetPath(entry.path().string());
				
				// XInput
				newContext->AddKeyBinding("XInput_A", mappings["XInput_A"]);
				newContext->AddKeyBinding("XInput_B", mappings["XInput_B"]);
				newContext->AddKeyBinding("XInput_X", mappings["XInput_X"]);
				newContext->AddKeyBinding("XInput_Y", mappings["XInput_Y"]);
				newContext->AddKeyBinding("XInput_LB", mappings["XInput_LB"]);
				newContext->AddKeyBinding("XInput_RB", mappings["XInput_RB"]);
				newContext->AddKeyBinding("XInput_ScreenShot", mappings["XInput_ScreenShot"]);
				newContext->AddKeyBinding("XInput_Start", mappings["XInput_Start"]);
				newContext->AddKeyBinding("XInput_LS", mappings["XInput_LS"]);
				newContext->AddKeyBinding("XInput_RS", mappings["XInput_RS"]);
				newContext->AddKeyBinding("XInput_Home", mappings["XInput_Home"]);
				newContext->AddKeyBinding("XInput_Tray", mappings["XInput_Tray"]);
				newContext->AddKeyBinding("XInput_DPadUp", mappings["XInput_DPadUp"]);
				newContext->AddKeyBinding("XInput_DPadDown", mappings["XInput_DPadDown"]);
				newContext->AddKeyBinding("XInput_DPadLeft", mappings["XInput_DPadLeft"]);
				newContext->AddKeyBinding("XInput_DPadRight", mappings["XInput_DPadRight"]);
				newContext->AddKeyBinding("XInput_LeftJoystickX", mappings["XInput_LeftJoystickX"]);
				newContext->AddKeyBinding("XInput_LeftJoystickY", mappings["XInput_LeftJoystickY"]);
				newContext->AddKeyBinding("XInput_RightJoystick", mappings["XInput_RightJoystick"]);
				newContext->AddKeyBinding("XInput_LT", mappings["XInput_LT"]);
				newContext->AddKeyBinding("XInput_RT", mappings["XInput_RT"]);

				//// Keyboard + Mouse
				// Directional
				newContext->AddKeyBinding("SDLK_UP", mappings["SDLK_UP"]);
				newContext->AddKeyBinding("SDLK_DOWN", mappings["SDLK_DOWN"]);
				newContext->AddKeyBinding("SDLK_LEFT", mappings["SDLK_LEFT"]);
				newContext->AddKeyBinding("SDLK_RIGHT", mappings["SDLK_RIGHT"]);
				// Letters
				newContext->AddKeyBinding("SDLK_a", mappings["SDLK_a"]);
				newContext->AddKeyBinding("SDLK_b", mappings["SDLK_b"]);
				newContext->AddKeyBinding("SDLK_c", mappings["SDLK_c"]);
				newContext->AddKeyBinding("SDLK_d", mappings["SDLK_d"]);
				newContext->AddKeyBinding("SDLK_e", mappings["SDLK_e"]);
				newContext->AddKeyBinding("SDLK_f", mappings["SDLK_f"]);
				newContext->AddKeyBinding("SDLK_g", mappings["SDLK_g"]);
				newContext->AddKeyBinding("SDLK_h", mappings["SDLK_h"]);
				newContext->AddKeyBinding("SDLK_i", mappings["SDLK_i"]);
				newContext->AddKeyBinding("SDLK_j", mappings["SDLK_j"]);
				newContext->AddKeyBinding("SDLK_k", mappings["SDLK_k"]);
				newContext->AddKeyBinding("SDLK_l", mappings["SDLK_l"]);
				newContext->AddKeyBinding("SDLK_m", mappings["SDLK_m"]);
				newContext->AddKeyBinding("SDLK_n", mappings["SDLK_n"]);
				newContext->AddKeyBinding("SDLK_o", mappings["SDLK_o"]);
				newContext->AddKeyBinding("SDLK_p", mappings["SDLK_p"]);
				newContext->AddKeyBinding("SDLK_q", mappings["SDLK_q"]);
				newContext->AddKeyBinding("SDLK_r", mappings["SDLK_r"]);
				newContext->AddKeyBinding("SDLK_s", mappings["SDLK_s"]);
				newContext->AddKeyBinding("SDLK_t", mappings["SDLK_t"]);
				newContext->AddKeyBinding("SDLK_u", mappings["SDLK_u"]);
				newContext->AddKeyBinding("SDLK_v", mappings["SDLK_v"]);
				newContext->AddKeyBinding("SDLK_w", mappings["SDLK_w"]);
				newContext->AddKeyBinding("SDLK_x", mappings["SDLK_x"]);
				newContext->AddKeyBinding("SDLK_y", mappings["SDLK_y"]);
				newContext->AddKeyBinding("SDLK_z", mappings["SDLK_z"]);

				// After all keys are set, create their Input Action bindings
				newContext->CreateInputActionBindings();

				// Add context to context managing vector
				mappingContexts.push_back(newContext);
			}
		}
	}

	std::shared_ptr<MappingContext> GetMappingContext(std::string contextName)
	{
		for (std::shared_ptr<MappingContext> mappingContext : mappingContexts)
			if (mappingContext->GetName() == contextName)
				return mappingContext;

		return nullptr;
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

	long GetEllapsedGameTimeInSec()
	{
		return F_Application->GetGameLoop()->TimeEllapsedInSec();
	}

	double GetEllapsedGameTimeInMs()
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