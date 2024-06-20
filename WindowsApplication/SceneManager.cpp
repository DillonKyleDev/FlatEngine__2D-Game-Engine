#include "SceneManager.h"
#include "Scene.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "ScriptComponent.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "json.hpp"
#include "TagList.h"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		this->loadedScene;
		animatorPreviewObjects = std::vector<std::shared_ptr<GameObject>>();
	}

	SceneManager::~SceneManager()
	{
	}

	std::shared_ptr<Scene> SceneManager::CreateNewScene()
	{
		// Remove loaded scene from memory
		loadedScene = nullptr;

		// Start up a new scene
		std::shared_ptr<Scene> freshScene = std::make_shared<Scene>();
		loadedScene = freshScene;

		FlatEngine::FlatGui::ResetHierarchyExpanderTracker();

		return loadedScene;
	}

	void SceneManager::SaveScene(std::shared_ptr<Scene> scene, std::string filePath)
	{
		loadedScenePath = filePath;

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filePath);

		// Delete old contents of the file
		file_obj.open(filePath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(filePath, std::ios::app);
 
		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;

		std::vector<std::shared_ptr<GameObject>> sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (int i = 0; i < sceneObjects.size(); i++)
			{
				// Finally, add the gameObject json to the sceneObjectsJsonArray
				sceneObjectsJsonArray.push_back(CreateJsonFromObject(sceneObjects[i]));
			}
		}
		else
		{
			sceneObjectsJsonArray.push_back("NULL");
		}

		// Recreate the GameObjects json object and add the array as the content
		json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SceneManager::SaveCurrentScene()
	{
		SaveScene(loadedScene, loadedScenePath);
	}

	void SceneManager::LoadScene(std::string filePath)
	{
		loadedScenePath = filePath;

		// Remove loaded scene from memory
		loadedScene = nullptr;

		// Start up a new scene
		std::shared_ptr<Scene> freshScene = std::make_shared<Scene>();
		loadedScene = freshScene;
		freshScene->SetName(FlatEngine::FlatGui::GetFilenameFromPath(filePath, false));
		freshScene->SetPath(filePath);

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filePath);

		// Open file in in mode
		file_obj.open(filePath, std::ios::in);

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

			if (fileContentJson["Scene GameObjects"][0] != "NULL")
			{
				auto firstObjectName = fileContentJson["Scene GameObjects"];

				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				// Loop through the saved GameObjects in the JSON file
				for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
				{
					// Add created GameObject to our freshScene
					std::shared_ptr<GameObject> loadedObject = CreateObjectFromJson(fileContentJson["Scene GameObjects"][i], freshScene);
					// If loaded object was a prefab, it will have been Instantiated, which already adds the object to the loaded scene
					if (!loadedObject->IsPrefab())
						freshScene->AddSceneObject(loadedObject);
				}
			}
		}

		// Assign our freshScene to the SceneManagers currently loadedScene member variable
		this->loadedScene = freshScene;

		if (GetObjectById(GetFocusedGameObjectID()) == nullptr)
			SetFocusedGameObjectID(-1);

		FlatEngine::FlatGui::ResetHierarchyExpanderTracker();
	}

	json SceneManager::CreateJsonFromObject(std::shared_ptr<GameObject> currentObject)
	{
		// Declare components array json object for components
		json componentsArray = json::array();

		std::vector<std::shared_ptr<Component>> components = currentObject->GetComponents();

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

		for (int c = 0; c < currentObject->GetChildren().size(); c++)
		{
			childrenArray.push_back(currentObject->GetChildren()[c]);
		}

		// Declare tags object for GameObject Tags	
		std::map <std::string, bool> tagList = currentObject->GetTagList()->GetTagsMap();

		json tagsObjectJson = json::object({
			{ "Player", tagList.at("Player") },
			{ "Enemy", tagList.at("Enemy") },
			{ "Npc", tagList.at("Npc") },
			{ "OnlyForPlayer", tagList.at("OnlyForPlayer") },
			{ "OnlyForEnemy", tagList.at("OnlyForEnemy") },
			{ "OnlyForNpc", tagList.at("OnlyForNpc") },
			{ "IgnorePlayer", tagList.at("IgnorePlayer") },
			{ "IgnoreEnemy", tagList.at("IgnoreEnemy") },
			{ "IgnoreNpc", tagList.at("IgnoreNpc") },
			{ "Projectile", tagList.at("Projectile") },
			{ "Terrain", tagList.at("Terrain") },
			{ "InteractableItem", tagList.at("InteractableItem") },
			{ "InteractableObject", tagList.at("InteractableObject") },
			{ "Item", tagList.at("Item") },
		});

		// Get object name
		std::string objectName = currentObject->GetName();
		Vector2 spawnLocation = currentObject->GetPrefabSpawnLocation();
		if (currentObject->HasComponent("Transform"))
			spawnLocation = currentObject->GetTransformComponent()->GetPosition();

		// Create Game Object Json data object
		json gameObjectJson = json::object({
			{ "_isPrefab", currentObject->IsPrefab() },
			{ "prefabName", currentObject->GetPrefabName() },
			{ "spawnLocationX", spawnLocation.x },
			{ "spawnLocationY", spawnLocation.y },
			{ "name", objectName },
			{ "id", currentObject->GetID() },
			{ "_isActive", currentObject->IsActive() },
			{ "parent", currentObject->GetParentID() },
			{ "children", childrenArray },
			{ "components", componentsArray },
			{ "tags", tagsObjectJson },
		});

		return gameObjectJson;
	}

	std::shared_ptr<GameObject> SceneManager::CreateObjectFromJson(json currentObjectJson, std::shared_ptr<Scene> loadedScene)
	{
		// Default values
		bool _isPrefab = false;
		std::string prefabName = "";
		std::string loadedName = "Name";
		Vector2 spawnLocation = Vector2(0, 0);
		long loadedID = -1;
		std::shared_ptr<TagList> tags = std::make_shared<TagList>();
		bool _isActive = true;
		long loadedParentID = -1;
		std::vector<long> loadedChildrenIDs = std::vector<long>();

		// Check name key exists
		if (currentObjectJson.contains("name"))
			loadedName = currentObjectJson["name"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'name' in object: " + loadedName);
		// Check ID key exists
		if (currentObjectJson.contains("id"))
			loadedID = currentObjectJson["id"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);

		// Keep loadedScene->nextGameObjectID updated to the highest GameObject ID loaded + 1 //
		if (loadedScene != nullptr && loadedID > loadedScene->nextGameObjectID)
			loadedScene->nextGameObjectID = loadedID + 1;

		// Check parent key exists
		if (currentObjectJson.contains("parent"))
			loadedParentID = currentObjectJson["parent"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'parent' in object: " + loadedName);
		// Check children key exists
		if (currentObjectJson.contains("children"))
		{
			// Add children
			for (int c = 0; c < currentObjectJson["children"].size(); c++)
			{
				loadedChildrenIDs.push_back(currentObjectJson["children"][c]);
			}
		}
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'children' in object: " + loadedName);

		// Check _isActive key exists
		if (currentObjectJson.contains("_isActive"))
			_isActive = currentObjectJson["_isActive"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);


		// Create new GameObject to load the data into
		std::shared_ptr<GameObject> loadedObject;

		// Check if it is a Prefab Object
		if (currentObjectJson.contains("_isPrefab"))
			_isPrefab = currentObjectJson["_isPrefab"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for '_isPrefab' in object: " + loadedName);
		// Get Prefab Object name
		if (currentObjectJson.contains("prefabName"))
			prefabName = currentObjectJson["prefabName"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'prefabName' in object: " + loadedName);
		// Get Prefab spawn location (SetOrigin() is taken care of by Instantiate() using the parentID parameter.)
		if (currentObjectJson.contains("spawnLocationX"))
			spawnLocation.x = currentObjectJson["spawnLocationX"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'spawnLocationX' in object: " + loadedName);
		if (currentObjectJson.contains("spawnLocationY"))
			spawnLocation.y = currentObjectJson["spawnLocationY"];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'spawnLocationY' in object: " + loadedName);

		// Instantiate Prefab if it is one using PrefabManager (keeps objects up-to-date with changes made to Prefab json)
		if (_isPrefab)
		{
			loadedObject = Instantiate(prefabName, spawnLocation, loadedParentID, loadedID);
		}
		else // If object does not originate from a Prefab, load it normally
		{
			loadedObject = std::make_shared<GameObject>(loadedParentID, loadedID);


			// Check _isActive key exists
			if (currentObjectJson.contains("_isActive"))
				_isActive = currentObjectJson["_isActive"];
			else
				FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);

			// Check tags key exists
			if (currentObjectJson.contains("tags"))
			{
				tags->SetTag("Player", currentObjectJson["tags"]["Player"]);
				tags->SetTag("Enemy", currentObjectJson["tags"]["Enemy"]);
				tags->SetTag("Npc", currentObjectJson["tags"]["Npc"]);
				tags->SetTag("OnlyForPlayer", currentObjectJson["tags"]["OnlyForPlayer"]);
				tags->SetTag("OnlyForEnemy", currentObjectJson["tags"]["OnlyForEnemy"]);
				tags->SetTag("OnlyForNpc", currentObjectJson["tags"]["OnlyForNpc"]);
				tags->SetTag("IgnorePlayer", currentObjectJson["tags"]["IgnorePlayer"]);
				tags->SetTag("IgnoreEnemy", currentObjectJson["tags"]["IgnoreEnemy"]);
				tags->SetTag("IgnoreNpc", currentObjectJson["tags"]["IgnoreNpc"]);
				tags->SetTag("Projectile", currentObjectJson["tags"]["Projectile"]);
				tags->SetTag("Terrain", currentObjectJson["tags"]["Terrain"]);
				tags->SetTag("InteractableItem", currentObjectJson["tags"]["InteractableItem"]);
				tags->SetTag("InteractableObject", currentObjectJson["tags"]["InteractableObject"]);
				tags->SetTag("Item", currentObjectJson["tags"]["Item"]);
			}
			else
				FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for 'tags' in object: " + loadedName);

			loadedObject->SetTagList(tags);

			float objectRotation = 0;

			// Loop through the components in this GameObjects json
			for (int j = 0; j < currentObjectJson["components"].size(); j++)
			{
				std::string type = "";
				if (currentObjectJson["components"][j].contains("type"))
					type = currentObjectJson["components"][j]["type"];
				else
					FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'type' in object: " + loadedName);

				long id = -1;
				bool _isCollapsed = true;
				bool _isActive = true;

				//Add each loaded component to the newly created GameObject
				if (type == "Transform")
				{
					// Default values
					float xOrigin = 0;
					float yOrigin = 0;
					float xScale = 1;
					float yScale = 1;
					float xPos = 0;
					float yPos = 0;
					float rotation = 0;

					std::shared_ptr<Transform> newTransform = loadedObject->AddTransformComponent();

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);


					// Origin
					if (currentObjectJson["components"][j].contains("xOrigin"))
						xOrigin = currentObjectJson["components"][j]["xOrigin"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xOrigin' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("yOrigin"))
						yOrigin = currentObjectJson["components"][j]["yOrigin"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yOrigin' in object: " + loadedName);


					// position
					if (currentObjectJson["components"][j].contains("xPos"))
						xPos = currentObjectJson["components"][j]["xPos"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xPos' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("yPos"))
						yPos = currentObjectJson["components"][j]["yPos"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yPos' in object: " + loadedName);


					// scale
					if (currentObjectJson["components"][j].contains("xScale"))
						xScale = currentObjectJson["components"][j]["xScale"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xScale' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("yScale"))
						yScale = currentObjectJson["components"][j]["yScale"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yScale' in object: " + loadedName);


					// rotation
					if (currentObjectJson["components"][j].contains("rotation"))
						rotation = currentObjectJson["components"][j]["rotation"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'rotation' in object: " + loadedName);


					newTransform->SetID(id);
					newTransform->SetCollapsed(_isCollapsed);
					newTransform->SetActive(_isActive);
					newTransform->SetOrigin(Vector2(xOrigin, yOrigin));
					newTransform->SetInitialPosition(Vector2(xPos, yPos));
					newTransform->SetScale(Vector2(xScale, yScale));
					newTransform->SetRotation(rotation);
					objectRotation = rotation;
				}
				else if (type == "Sprite")
				{
					std::shared_ptr<Sprite> newSprite = loadedObject->AddSpriteComponent();

					// Default values
					std::string path = "";
					float xScale = 1;
					float yScale = 1;
					float xOffset = 0;
					float yOffset = 0;
					int renderOrder = 0;
					std::string pivotPoint = "Center";
					Vector4 tintColor = Vector4(1, 1, 1, 1);

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);


					if (currentObjectJson["components"][j].contains("path"))
						path = currentObjectJson["components"][j]["path"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'path' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("xScale"))
						xScale = currentObjectJson["components"][j]["xScale"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xScale' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("yScale"))
						yScale = currentObjectJson["components"][j]["yScale"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yScale' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("pivotPoint"))
						pivotPoint = currentObjectJson["components"][j]["pivotPoint"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'pivotPoint' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("xOffset"))
						xOffset = currentObjectJson["components"][j]["xOffset"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xOffset' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("yOffset"))
						yOffset = currentObjectJson["components"][j]["yOffset"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yOffset' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("renderOrder"))
						renderOrder = currentObjectJson["components"][j]["renderOrder"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'renderOrder' in object: " + loadedName);

					// Tint color
					if (currentObjectJson["components"][j].contains("tintColorX"))
						tintColor.x = currentObjectJson["components"][j]["tintColorX"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'tintColorX' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("tintColorY"))
						tintColor.y = currentObjectJson["components"][j]["tintColorY"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'tintColorY' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("tintColorZ"))
						tintColor.z = currentObjectJson["components"][j]["tintColorZ"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'tintColorZ' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("tintColorW"))
						tintColor.w = currentObjectJson["components"][j]["tintColorW"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'tintColorW' in object: " + loadedName);

					Vector2 scale = Vector2(xScale, yScale);

					newSprite->SetID(id);
					newSprite->SetCollapsed(_isCollapsed);
					newSprite->SetActive(_isActive);
					newSprite->SetTexture(path);
					newSprite->SetScale(scale);
					newSprite->SetPivotPoint(pivotPoint);
					newSprite->SetOffset(Vector2(xOffset, yOffset));
					newSprite->SetRenderOrder(renderOrder);
					newSprite->SetTintColor(tintColor);
				}
				else if (type == "Camera")
				{
					std::shared_ptr<Camera> newCamera = loadedObject->AddCameraComponent();

					// Default values
					float width = 400;
					float height = 300;
					bool _isPrimaryCamera = false;
					float zoom = 1;
					float f_red = 1;
					float f_green = 1;
					float f_blue = 1;
					float f_alpha = 1;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);

					// Width and height
					if (currentObjectJson["components"][j].contains("width"))
						width = currentObjectJson["components"][j]["width"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'width' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("height"))
						height = currentObjectJson["components"][j]["height"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'height' in object: " + loadedName);
					// Is primaryCamera
					if (currentObjectJson["components"][j].contains("_isPrimaryCamera"))
						_isPrimaryCamera = currentObjectJson["components"][j]["_isPrimaryCamera"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isPrimaryCamera' in object: " + loadedName);
					// Zoom
					if (currentObjectJson["components"][j].contains("zoom"))
						zoom = currentObjectJson["components"][j]["zoom"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'zoom' in object: " + loadedName);
					// Frustrum RGBA values
					if (currentObjectJson["components"][j].contains("frustrumRed"))
						f_red = currentObjectJson["components"][j]["frustrumRed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumRed' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("frustrumGreen"))
						f_green = currentObjectJson["components"][j]["frustrumGreen"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumGreen' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("frustrumBlue"))
						f_blue = currentObjectJson["components"][j]["frustrumBlue"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumBlue' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("frustrumAlpha"))
						f_alpha = currentObjectJson["components"][j]["frustrumAlpha"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumAlpha' in object: " + loadedName);

					Vector4 frustrumColor = Vector4(f_red, f_green, f_blue, f_alpha);

					newCamera->SetID(id);
					newCamera->SetCollapsed(_isCollapsed);
					newCamera->SetActive(_isActive);
					newCamera->SetDimensions(width, height);
					newCamera->SetPrimaryCamera(_isPrimaryCamera);
					newCamera->SetZoom(zoom);
					newCamera->SetFrustrumColor(frustrumColor);

					// If this camera is the primary camera, set it in the Scene as the primaryCamera
					if (_isPrimaryCamera && loadedScene != nullptr)
						loadedScene->SetPrimaryCamera(newCamera);
				}
				else if (type == "Script")
				{
					std::shared_ptr<ScriptComponent> newScript = loadedObject->AddScriptComponent();

					// Default values
					std::string attachedScript = "";

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);


					if (currentObjectJson["components"][j].contains("attachedScript"))
						attachedScript = currentObjectJson["components"][j]["attachedScript"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'attachedScript' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);

					newScript->SetID(id);
					newScript->SetCollapsed(_isCollapsed);
					newScript->SetActive(_isActive);
					newScript->SetAttachedScript(attachedScript);
				}
				else if (type == "Button")
				{
					std::shared_ptr<Button> newButton = loadedObject->AddButtonComponent();

					// Default values
					float activeWidth = 2;
					float activeHeight = 2;
					Vector2 activeOffset = { 0, 0 };
					int activeLayer = 0;
					std::string attachedScript = "";

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("activeWidth"))
						activeWidth = currentObjectJson["components"][j]["activeWidth"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeWidth' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("activeHeight"))
						activeHeight = currentObjectJson["components"][j]["activeHeight"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeHeight' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("activeOffsetX"))
						activeOffset.x = currentObjectJson["components"][j]["activeOffsetX"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetX' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("activeOffsetY"))
						activeOffset.y = currentObjectJson["components"][j]["activeOffsetY"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetY' in object: " + loadedName);
					// Active Layer
					if (currentObjectJson["components"][j].contains("activeLayer"))
						activeLayer = currentObjectJson["components"][j]["activeLayer"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeLayer' in object: " + loadedName);

					newButton->SetID(id);
					newButton->SetCollapsed(_isCollapsed);
					newButton->SetActive(_isActive);
					newButton->SetActiveDimensions(activeWidth, activeHeight);
					newButton->SetActiveOffset(activeOffset);
					newButton->SetActiveLayer(activeLayer);
				}
				else if (type == "Canvas")
				{
					std::shared_ptr<Canvas> newCanvas = loadedObject->AddCanvasComponent();

					// Default values
					float canvasWidth = 5;
					float canvasHeight = 3;
					int layerNumber = -1;
					bool _blocksLayers = true;


					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					if (currentObjectJson["components"][j].contains("width"))
						canvasWidth = currentObjectJson["components"][j]["width"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'width' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("height"))
						canvasHeight = currentObjectJson["components"][j]["height"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'height' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("layerNumber"))
						layerNumber = currentObjectJson["components"][j]["layerNumber"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'layerNumber' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("_blocksLayers"))
						_blocksLayers = currentObjectJson["components"][j]["_blocksLayers"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_blocksLayers' in object: " + loadedName);

					// Assign values to the new Canvas
					newCanvas->SetID(id);
					newCanvas->SetCollapsed(_isCollapsed);
					newCanvas->SetActive(_isActive);
					newCanvas->SetDimensions(canvasWidth, canvasHeight);
					newCanvas->SetLayerNumber(layerNumber);
					newCanvas->SetBlocksLayers(_blocksLayers);
				}
				else if (type == "Animation")
				{
					std::shared_ptr<Animation> newAnimation = loadedObject->AddAnimationComponent();

					// Default values
					std::string path = "";

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// Path
					if (currentObjectJson["components"][j].contains("path"))
						path = currentObjectJson["components"][j]["path"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'path' in object: " + loadedName);

					// Assign values to the new Canvas
					newAnimation->SetID(id);
					newAnimation->SetCollapsed(_isCollapsed);
					newAnimation->SetActive(_isActive);
					newAnimation->Stop();
					newAnimation->SetAnimationPath(path);
				}
				else if (type == "Audio")
				{
					std::shared_ptr<Audio> newAudio = loadedObject->AddAudioComponent();

					// Default values
					bool _isMusic = false;
					std::string path = "";


					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					if (currentObjectJson["components"][j].contains("_isMusic"))
						_isMusic = currentObjectJson["components"][j]["_isMusic"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isMusic' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("path"))
						path = currentObjectJson["components"][j]["path"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'path' in object: " + loadedName);

					// Assign values to the new Canvas
					newAudio->SetID(id);
					newAudio->SetCollapsed(_isCollapsed);
					newAudio->SetActive(_isActive);
					newAudio->SetPath(path);
					newAudio->SetIsMusic(_isMusic);
					if (_isMusic)
						newAudio->LoadMusic(path);
					else
						newAudio->LoadEffect(path);
				}
				else if (type == "Text")
				{
					std::shared_ptr<Text> newText = loadedObject->AddTextComponent();

					// Default values
					std::string fontPath = "";
					int fontSize = 40;
					std::string text = "Text Component";
					Uint8 f_red = 1;
					Uint8 f_green = 1;
					Uint8 f_blue = 1;
					Uint8 f_alpha = 1;
					Vector2 offset = { 0, 0 };
					int renderOrder = 0;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// Text Properties
					if (currentObjectJson["components"][j].contains("fontPath"))
						fontPath = currentObjectJson["components"][j]["fontPath"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'fontPath' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("fontSize"))
						fontSize = currentObjectJson["components"][j]["fontSize"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'fontSize' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("text"))
						text = currentObjectJson["components"][j]["text"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'text' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("renderOrder"))
						renderOrder = currentObjectJson["components"][j]["renderOrder"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'renderOrder' in object: " + loadedName);
					// Font RGBA values
					if (currentObjectJson["components"][j].contains("f_red"))
						f_red = currentObjectJson["components"][j]["f_red"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_red' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("f_green"))
						f_green = currentObjectJson["components"][j]["f_green"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_green' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("f_blue"))
						f_blue = currentObjectJson["components"][j]["f_blue"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_blue' in object: " + loadedName);
					// Offset
					if (currentObjectJson["components"][j].contains("xOffset"))
						offset.x = currentObjectJson["components"][j]["xOffset"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xOffset' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("yOffset"))
						offset.y = currentObjectJson["components"][j]["yOffset"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yOffset' in object: " + loadedName);

					SDL_Color fontColor;
					fontColor.r = f_red;
					fontColor.g = f_green;
					fontColor.b = f_blue;

					newText->SetID(id);
					newText->SetCollapsed(_isCollapsed);
					newText->SetActive(_isActive);
					newText->SetFontPath(fontPath);
					newText->SetFontSize(fontSize);
					newText->SetColor(fontColor);
					newText->SetText(text);
					newText->SetOffset(offset);
					newText->SetRenderOrder(renderOrder);
					newText->LoadText();
				}
				else if (type == "CharacterController")
				{
					std::shared_ptr<CharacterController> newCharacterController = loadedObject->AddCharacterControllerComponent();

					// Default values
					float maxAcceleration = 1;
					float maxSpeed = 2;
					float airControl = 1;
					bool _isMoving = false;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// CharacterController Properties
					if (currentObjectJson["components"][j].contains("maxAcceleration"))
						maxAcceleration = currentObjectJson["components"][j]["maxAcceleration"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'maxAcceleration' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("maxSpeed"))
						maxSpeed = currentObjectJson["components"][j]["maxSpeed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'maxSpeed' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("airControl"))
						airControl = currentObjectJson["components"][j]["airControl"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'airControl' in object: " + loadedName);

					newCharacterController->SetID(id);
					newCharacterController->SetCollapsed(_isCollapsed);
					newCharacterController->SetActive(_isActive);
					newCharacterController->SetMaxAcceleration(maxAcceleration);
					newCharacterController->SetMaxSpeed(maxSpeed);
				}
				else if (type == "BoxCollider")
				{
					std::shared_ptr<BoxCollider> newBoxCollider = loadedObject->AddBoxColliderComponent();

					// Default values
					float activeWidth = 2;
					float activeHeight = 2;
					Vector2 activeOffset = Vector2(0, 0);
					bool _isContinuous = true;
					bool _isStatic = false;
					bool _isSolid = true;
					int activeLayer = 0;
					bool _showActiveRadius = false;
					bool _isComposite = false;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// BoxCollider Properties
					if (currentObjectJson["components"][j].contains("activeWidth"))
						activeWidth = currentObjectJson["components"][j]["activeWidth"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeWidth' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeHeight"))
						activeHeight = currentObjectJson["components"][j]["activeHeight"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeHeight' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeOffsetX"))
						activeOffset.x = currentObjectJson["components"][j]["activeOffsetX"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetX' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeOffsetY"))
						activeOffset.y = currentObjectJson["components"][j]["activeOffsetY"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetY' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isContinuous"))
						_isContinuous = currentObjectJson["components"][j]["_isContinuous"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isContinious' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isStatic"))
						_isStatic = currentObjectJson["components"][j]["_isStatic"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isStatic' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isSolid"))
						_isSolid = currentObjectJson["components"][j]["_isSolid"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isSolid' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeLayer"))
						activeLayer = currentObjectJson["components"][j]["activeLayer"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeLayer' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_showActiveRadius"))
						_showActiveRadius = currentObjectJson["components"][j]["_showActiveRadius"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_showActiveRadius' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isComposite"))
						_isComposite = currentObjectJson["components"][j]["_isComposite"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isComposite' in object: " + loadedName);

					newBoxCollider->SetID(id);
					newBoxCollider->SetCollapsed(_isCollapsed);
					newBoxCollider->SetActive(_isActive);
					newBoxCollider->SetActiveDimensions(activeWidth, activeHeight);
					newBoxCollider->SetActiveOffset(activeOffset);
					newBoxCollider->SetIsContinuous(_isContinuous);
					newBoxCollider->SetIsStatic(_isStatic);
					newBoxCollider->SetIsSolid(_isSolid);
					newBoxCollider->SetActiveLayer(activeLayer);
					newBoxCollider->SetRotation(objectRotation);
					newBoxCollider->SetShowActiveRadius(_showActiveRadius);
					newBoxCollider->SetIsComposite(_isComposite);
				}
				else if (type == "CircleCollider")
				{
					std::shared_ptr<CircleCollider> newCircleCollider = loadedObject->AddCircleColliderComponent();

					// Default values
					float activeRadius = 1.5;
					Vector2 activeOffset = Vector2(0, 0);
					bool _isContinuous = true;
					bool _isStatic = false;
					bool _isSolid = true;
					int activeLayer = 0;
					bool _isComposite = false;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// CircleCollider Properties
					if (currentObjectJson["components"][j].contains("activeRadius"))
						activeRadius = currentObjectJson["components"][j]["activeRadius"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeRadius' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeOffsetX"))
						activeOffset.x = currentObjectJson["components"][j]["activeOffsetX"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetX' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeOffsetY"))
						activeOffset.y = currentObjectJson["components"][j]["activeOffsetY"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetY' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isContinuous"))
						_isContinuous = currentObjectJson["components"][j]["_isContinuous"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isContinious' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isStatic"))
						_isStatic = currentObjectJson["components"][j]["_isStatic"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isStatic' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isSolid"))
						_isSolid = currentObjectJson["components"][j]["_isSolid"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isSolid' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("activeLayer"))
						activeLayer = currentObjectJson["components"][j]["activeLayer"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeLayer' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isComposite"))
						_isComposite = currentObjectJson["components"][j]["_isComposite"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isComposite' in object: " + loadedName);

					newCircleCollider->SetID(id);
					newCircleCollider->SetCollapsed(_isCollapsed);
					newCircleCollider->SetActive(_isActive);
					newCircleCollider->SetActiveRadiusGrid(activeRadius);
					newCircleCollider->SetActiveOffset(activeOffset);
					newCircleCollider->SetIsContinuous(_isContinuous);
					newCircleCollider->SetIsStatic(_isStatic);
					newCircleCollider->SetIsSolid(_isSolid);
					newCircleCollider->SetActiveLayer(activeLayer);
					newCircleCollider->SetIsComposite(_isComposite);
				}
				else if (type == "RigidBody")
				{
					std::shared_ptr<RigidBody> newRigidBody = std::static_pointer_cast<RigidBody>(loadedObject->AddComponent(ComponentTypes::RigidBody));

					// Default values
					float mass = 1.0f;
					float angularDrag = 1.0f;
					float gravity = 1.0f;
					float friction = 1.0f;
					float equilibriumForce = 2.0f;
					float terminalVelocity = 0.4f;
					float windResistance = 1.0f;
					bool _isKinematic = false;
					bool _isStatic = false;

					// Load ID
					if (currentObjectJson["components"][j].contains("id"))
						id = currentObjectJson["components"][j]["id"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: " + loadedName);
					// Load _isCollapsed
					if (currentObjectJson["components"][j].contains("_isCollapsed"))
						_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: " + loadedName);
					// Load _isActive
					if (currentObjectJson["components"][j].contains("_isActive"))
						_isActive = currentObjectJson["components"][j]["_isActive"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: " + loadedName);



					// CharacterController Properties
					if (currentObjectJson["components"][j].contains("mass"))
						mass = currentObjectJson["components"][j]["mass"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'mass' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("angularDrag"))
						angularDrag = currentObjectJson["components"][j]["angularDrag"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'angularDrag' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("gravity"))
						gravity = currentObjectJson["components"][j]["gravity"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'gravity' in object: " + loadedName);

					if (currentObjectJson["components"][j].contains("friction"))
						friction = currentObjectJson["components"][j]["friction"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'friction' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("equilibriumForce"))
						equilibriumForce = currentObjectJson["components"][j]["equilibriumForce"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'equilibriumForce' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("terminalVelocity"))
						terminalVelocity = currentObjectJson["components"][j]["terminalVelocity"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'terminalVelocity' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("windResistance"))
						windResistance = currentObjectJson["components"][j]["windResistance"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'windResistance' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isKinematic"))
						_isKinematic = currentObjectJson["components"][j]["_isKinematic"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isKinematic' in object: " + loadedName);
					if (currentObjectJson["components"][j].contains("_isStatic"))
						_isStatic = currentObjectJson["components"][j]["_isStatic"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isStatic' in object: " + loadedName);


					newRigidBody->SetID(id);
					newRigidBody->SetCollapsed(_isCollapsed);
					newRigidBody->SetActive(_isActive);
					newRigidBody->SetMass(mass);
					newRigidBody->SetAngularDrag(angularDrag);
					newRigidBody->SetGravity(gravity);
					newRigidBody->SetFriction(friction);
					newRigidBody->SetWindResistance(windResistance);
					newRigidBody->SetEquilibriumForce(equilibriumForce);
					newRigidBody->SetTerminalVelocity(terminalVelocity);
					newRigidBody->SetIsKinematic(_isKinematic);
					newRigidBody->SetIsStatic(_isStatic);
				}

				if (loadedScene != nullptr && id > loadedScene->nextComponentID)
					loadedScene->nextComponentID = id + 1;
			}
		}

		loadedObject->SetName(loadedName);
		loadedObject->SetActive(_isActive);

		// Add children
		for (int c = 0; c < currentObjectJson["children"].size(); c++)
		{
			loadedObject->AddChild(loadedChildrenIDs[c]);
		}

		return loadedObject;
	}

	std::shared_ptr<Scene> SceneManager::GetLoadedScene()
	{
		return this->loadedScene;
	}
	
	std::string SceneManager::GetLoadedScenePath()
	{
		return loadedScenePath;
	}
	
	void SceneManager::SaveAnimationPreviewObjects()
	{
		if (loadedScene->GetAnimatorPreviewObjects().size() > 0)
			animatorPreviewObjects = loadedScene->GetAnimatorPreviewObjects();
	}
	
	void SceneManager::LoadAnimationPreviewObjects()
	{
		loadedScene->SetAnimatorPreviewObjects(animatorPreviewObjects);
	}
}