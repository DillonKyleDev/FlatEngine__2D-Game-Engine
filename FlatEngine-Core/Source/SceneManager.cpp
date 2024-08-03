#include "SceneManager.h"
#include "Scene.h"
#include "Button.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Canvas.h"
#include "ScriptComponent.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "TagList.h"

#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		this->loadedScene;
		animatorPreviewObjects = std::vector<GameObject*>();
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

		std::map<long, GameObject> sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
			{
				// Finally, add the gameObject json to the sceneObjectsJsonArray
				sceneObjectsJsonArray.push_back(CreateJsonFromObject(iter->second));
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
		std::string fileName = GetFilenameFromPath(filePath, false);
		freshScene->SetName(fileName);
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
					GameObject loadedObject = CreateObjectFromJson(fileContentJson["Scene GameObjects"][i], freshScene);
					// If loaded object was a prefab, it will have been Instantiated, which already adds the object to the loaded scene
					if (!loadedObject.IsPrefab())
						freshScene->AddSceneObject(loadedObject);
				}
			}
		}

		// Assign our freshScene to the SceneManagers currently loadedScene member variable
		this->loadedScene = freshScene;

		F_Application->OnLoadScene(fileName);
	}

	json SceneManager::CreateJsonFromObject(GameObject currentObject)
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
			spawnLocation = currentObject.GetTransformComponent()->GetPosition();

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
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return contains;
	}
	float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName)
	{
		float value = 0;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
	{
		int value = 0;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	long CheckJsonLong(json obj, std::string checkFor, std::string loadedName)
	{
		long value = -1;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName)
	{
		bool value = false;
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}
	std::string SceneManager::CheckJsonString(json obj, std::string checkFor, std::string loadedName)
	{
		std::string value = "Failed to load.";
		if (obj.contains(checkFor))
			value = obj[checkFor];
		else
			FlatEngine::LogString("SceneManager::Load() - Saved scene json does not contain a value for " + checkFor + " in object : " + loadedName);
		return value;
	}

	GameObject SceneManager::CreateObjectFromJson(json objectJson, std::shared_ptr<Scene> loadedScene)
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

		// Keep loadedScene->nextGameObjectID updated to the highest GameObject ID loaded + 1
		if (loadedScene != nullptr && loadedID > loadedScene->nextGameObjectID)
			loadedScene->nextGameObjectID = loadedID + 1;

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
			if (JsonContains(objectJson, "tags", objectName))
			{
				json tagsObj = objectJson["tags"];
				tags.SetTag("Player", CheckJsonBool(tagsObj, "Player", objectName));
				tags.SetTag("Enemy", CheckJsonBool(tagsObj, "Enemy", objectName));
				tags.SetTag("Npc", CheckJsonBool(tagsObj, "Npc", objectName));
				tags.SetTag("Terrain", CheckJsonBool(tagsObj, "Terrain", objectName));			
				tags.SetTag("PlayerTrigger", CheckJsonBool(tagsObj, "PlayerTrigger", objectName));
				tags.SetTag("EnemyTrigger", CheckJsonBool(tagsObj, "EnemyTrigger", objectName));
				tags.SetTag("NpcTrigger", CheckJsonBool(tagsObj, "NpcTrigger", objectName));
				tags.SetTag("EnvironmentalTrigger", CheckJsonBool(tagsObj, "EnvironmentalTrigger", objectName));
				tags.SetTag("TerrainTrigger", CheckJsonBool(tagsObj, "TerrainTrigger", objectName));
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName));
				tags.SetTag("PlayerDamage", CheckJsonBool(tagsObj, "PlayerDamage", objectName));
				tags.SetTag("EnemyDamage", CheckJsonBool(tagsObj, "EnemyDamage", objectName));
				tags.SetTag("EnvironmentalDamage", CheckJsonBool(tagsObj, "EnvironmentalDamage", objectName));
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName));
				tags.SetTag("InteractableItem", CheckJsonBool(tagsObj, "InteractableItem", objectName));
				tags.SetTag("InteractableObject", CheckJsonBool(tagsObj, "InteractableObject", objectName));
				tags.SetTag("Item", CheckJsonBool(tagsObj, "Item", objectName));
			}
			if (JsonContains(objectJson, "ignoreTags", objectName))
			{
				json ignoreTags = objectJson["ignoreTags"];
				tags.SetIgnore("Player", CheckJsonBool(ignoreTags, "Player", objectName));
				tags.SetIgnore("Enemy", CheckJsonBool(ignoreTags, "Enemy", objectName));
				tags.SetIgnore("Npc", CheckJsonBool(ignoreTags, "Npc", objectName));
				tags.SetIgnore("Terrain", CheckJsonBool(ignoreTags, "Terrain", objectName));
				tags.SetIgnore("PlayerTrigger", CheckJsonBool(ignoreTags, "PlayerTrigger", objectName));
				tags.SetIgnore("EnemyTrigger", CheckJsonBool(ignoreTags, "EnemyTrigger", objectName));
				tags.SetIgnore("NpcTrigger", CheckJsonBool(ignoreTags, "NpcTrigger", objectName));
				tags.SetIgnore("EnvironmentalTrigger", CheckJsonBool(ignoreTags, "EnvironmentalTrigger", objectName));
				tags.SetIgnore("TerrainTrigger", CheckJsonBool(ignoreTags, "TerrainTrigger", objectName));
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName));
				tags.SetIgnore("PlayerDamage", CheckJsonBool(ignoreTags, "PlayerDamage", objectName));
				tags.SetIgnore("EnemyDamage", CheckJsonBool(ignoreTags, "EnemyDamage", objectName));
				tags.SetIgnore("EnvironmentalDamage", CheckJsonBool(ignoreTags, "EnvironmentalDamage", objectName));
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName));
				tags.SetIgnore("InteractableItem", CheckJsonBool(ignoreTags, "InteractableItem", objectName));
				tags.SetIgnore("InteractableObject", CheckJsonBool(ignoreTags, "InteractableObject", objectName));
				tags.SetIgnore("Item", CheckJsonBool(ignoreTags, "Item", objectName));
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
					newSprite->SetTexture(CheckJsonString(componentJson, "path", objectName));
					newSprite->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					newSprite->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
					newSprite->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newSprite->SetTintColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
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

					// If this camera is the primary camera, set it in the Scene as the primaryCamera
					if (_isPrimaryCamera && loadedScene != nullptr)
						loadedScene->SetPrimaryCamera(newCamera);
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

				if (loadedScene != nullptr && id > loadedScene->nextComponentID)
					loadedScene->nextComponentID = id + 1;
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