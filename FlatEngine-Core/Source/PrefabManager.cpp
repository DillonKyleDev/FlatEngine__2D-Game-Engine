#include "PrefabManager.h"
#include "FlatEngine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "transform.h"
#include "RigidBody.h"
#include "sprite.h"
#include "BoxCollider.h"
#include "CharacterController.h"
#include "Canvas.h"
#include "Button.h"
#include "Text.h"
#include "Camera.h"
#include "Script.h"
#include "Scene.h"
#include "GameLoop.h"
#include "Audio.h"
#include "Vector2.h"

#include <fstream>

namespace FlatEngine
{
	PrefabManager::PrefabManager()
	{
		m_prefabs = std::map<std::string, Prefab>();
	}

	PrefabManager::~PrefabManager()
	{
	}

	void PrefabManager::CreatePrefabFromJson(json objectJson, Prefab &prefab)
	{
		GameObjectPrefabData object;
		object.name = CheckJsonString(objectJson, "name", "Name");
		std::string objectName = object.name;
		object.b_isActive = CheckJsonBool(objectJson, "_isActive", objectName);
		object.ID = CheckJsonLong(objectJson, "id", objectName);
		object.parentID = CheckJsonLong(objectJson, "parent", objectName);		
		TagList tags = TagList();

		if (JsonContains(objectJson, "children", objectName))
		{
			for (int c = 0; c < objectJson["children"].size(); c++)
			{
				object.childrenIDs.push_back(objectJson["children"][c]);
			}
		}

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
		
		object.tagList = tags;


		float objectRotation = 0;
		// Loop through the components in this Prefabs json
		for (int j = 0; j < objectJson["components"].size(); j++)
		{
			json componentJson = objectJson["components"][j];
			std::string type = CheckJsonString(componentJson, "type", objectName);
			long componentID = CheckJsonLong(componentJson, "id", objectName);
			bool b_isCollapsed = CheckJsonBool(componentJson, "_isCollapsed", objectName);
			bool b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);

			// connect this component to the object being saved
			object.componentIDs.push_back(componentID);

			//Add each loaded component to the newly created GameObject
			if (type == "Transform")
			{
				std::shared_ptr<TransformPrefabData> transform = std::make_shared<TransformPrefabData>();
				transform->type = "Transform";
				transform->id = componentID;
				transform->b_isActive = b_isActive;
				transform->b_isCollapsed = b_isCollapsed;
				transform->ownerId = object.ID;
				transform->rotation = CheckJsonFloat(componentJson, "rotation", objectName);
				transform->origin = Vector2(CheckJsonFloat(componentJson, "xOrigin", objectName), CheckJsonFloat(componentJson, "yOrigin", objectName));
				transform->scale = Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName));
				transform->rotation = CheckJsonFloat(componentJson, "rotation", objectName);
				objectRotation = transform->rotation;

				prefab.components.emplace(componentID, transform);
			}
			else if (type == "Sprite")
			{
				std::shared_ptr<SpritePrefabData> sprite = std::make_shared<SpritePrefabData>();
				sprite->type = "Sprite";
				sprite->id = componentID;
				sprite->b_isActive = b_isActive;
				sprite->b_isCollapsed = b_isCollapsed;
				sprite->ownerId = object.ID;
				sprite->path = CheckJsonString(componentJson, "path", objectName);
				std::string pivotPoint = "Center";
				if (CheckJsonString(componentJson, "pivotPoint", objectName) != "")
					pivotPoint = CheckJsonString(componentJson, "pivotPoint", objectName);
				sprite->pivotPoint = pivotPoint;
				sprite->scale = Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName));
				sprite->offset = Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName));
				sprite->renderOrder = CheckJsonInt(componentJson, "renderOrder", objectName);
				sprite->tintColor = Vector4(
					CheckJsonFloat(componentJson, "tintColorX", objectName),
					CheckJsonFloat(componentJson, "tintColorY", objectName),
					CheckJsonFloat(componentJson, "tintColorZ", objectName),
					CheckJsonFloat(componentJson, "tintColorW", objectName)
				);

				prefab.components.emplace(componentID, sprite);
			}
			else if (type == "Camera")
			{
				std::shared_ptr<CameraPrefabData> camera = std::make_shared<CameraPrefabData>();
				camera->type = "Camera";
				camera->id = componentID;
				camera->b_isActive = b_isActive;
				camera->b_isCollapsed = b_isCollapsed;
				camera->ownerId = object.ID;
				camera->width = CheckJsonFloat(componentJson, "width", objectName);
				camera->height = CheckJsonFloat(componentJson, "height", objectName);
				camera->b_isPrimaryCamera = CheckJsonBool(componentJson, "_isPrimaryCamera", objectName);
				camera->zoom = CheckJsonFloat(componentJson, "zoom", objectName);
				camera->frustrumColor = Vector4(
					CheckJsonFloat(componentJson, "frustrumRed", objectName),
					CheckJsonFloat(componentJson, "frustrumGreen", objectName),
					CheckJsonFloat(componentJson, "frustrumBlue", objectName),
					CheckJsonFloat(componentJson, "frustrumAlpha", objectName)
				);
				camera->b_follow = CheckJsonBool(componentJson, "_follow", objectName);
				camera->followSmoothing = CheckJsonFloat(componentJson, "followSmoothing", objectName);
				camera->toFollowID = CheckJsonLong(componentJson, "following", objectName);

				prefab.components.emplace(componentID, camera);
			}
			else if (type == "Script")
			{
				std::shared_ptr<ScriptPrefabData> script = std::make_shared<ScriptPrefabData>();	
				script->type = "Script";
				script->id = componentID;
				script->b_isActive = b_isActive;
				script->b_isCollapsed = b_isCollapsed;
				script->ownerId = object.ID;
				script->attachedScript = CheckJsonString(componentJson, "attachedScript", objectName);

				prefab.components.emplace(componentID, script);
			}
			else if (type == "Button")
			{
				//std::shared_ptr<ButtonPrefabData> script = std::make_shared<ScriptPrefabData>();
				//script->type = "Script";
				//newButton->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
				//newButton->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
				//newButton->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
			}
			else if (type == "Canvas")
			{
				//Canvas* newCanvas = loadedObject.AddCanvas(id, _isActive, _isCollapsed);
				//newCanvas->SetDimensions(CheckJsonFloat(componentJson, "canvasWidth", objectName), CheckJsonFloat(componentJson, "canvasHeight", objectName));
				//newCanvas->SetLayerNumber(CheckJsonInt(componentJson, "layerNumber", objectName));
				//newCanvas->SetBlocksLayers(CheckJsonBool(componentJson, "_blocksLayers", objectName));
			}
			else if (type == "Animation")
			{
				//Animation* newAnimation = loadedObject->AddAnimation(id, _isActive, _isCollapsed);
				//newAnimation->SetAnimationPath(CheckJsonString(componentJson, "path", objectName));
			}
			else if (type == "Audio")
			{
				//Audio* newAudio = loadedObject.AddAudio(id, _isActive, _isCollapsed);

				//// Get Sounds data
				//if (JsonContains(componentJson, "soundData", objectName))
				//{
				//	for (int sound = 0; sound < componentJson["soundData"].size(); sound++)
				//	{
				//		json tileJson = componentJson["soundData"][sound];
				//		std::string path = CheckJsonString(tileJson, "path", objectName);
				//		std::string name = CheckJsonString(tileJson, "name", objectName);
				//		bool b_isMusic = CheckJsonBool(tileJson, "b_isMusic", objectName);

				//		newAudio->AddSound(name, path, b_isMusic);
				//	}
				//}
			}
			else if (type == "Text")
			{
				//Text* newText = loadedObject.AddText(id, _isActive, _isCollapsed);
				//newText->SetFontPath(CheckJsonString(componentJson, "fontPath", objectName));
				//newText->SetFontSize(CheckJsonInt(componentJson, "fontSize", objectName));
				//newText->SetColor(Vector4(
				//	CheckJsonFloat(componentJson, "f_red", objectName),
				//	CheckJsonFloat(componentJson, "f_green", objectName),
				//	CheckJsonFloat(componentJson, "f_blue", objectName),
				//	CheckJsonFloat(componentJson, "f_alpha", objectName)
				//));
				//newText->SetText(CheckJsonString(componentJson, "text", objectName));
				//newText->SetOffset(Vector2(CheckJsonFloat(componentJson, "offsetX", objectName), CheckJsonFloat(componentJson, "offsetY", objectName)));
				//newText->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
				//newText->LoadText();
			}
			else if (type == "CharacterController")
			{
				//CharacterController* newCharacterController = loadedObject.AddCharacterController(id, _isActive, _isCollapsed);
				//newCharacterController->SetMaxAcceleration(CheckJsonFloat(componentJson, "maxAcceleration", objectName));
				//newCharacterController->SetMaxSpeed(CheckJsonFloat(componentJson, "maxSpeed", objectName));
				//newCharacterController->SetAirControl(CheckJsonFloat(componentJson, "airControl", objectName));
			}
			else if (type == "BoxCollider")
			{
				std::shared_ptr<BoxColliderPrefabData> boxCollider = std::make_shared<BoxColliderPrefabData>();
				boxCollider->type = "BoxCollider";
				boxCollider->id = componentID;
				boxCollider->b_isActive = b_isActive;
				boxCollider->b_isCollapsed = b_isCollapsed;
				boxCollider->ownerId = object.ID;
				boxCollider->b_tileMapCollider = CheckJsonBool(componentJson, "_isTileMapCollider", objectName);
				boxCollider->activeDimensions = Vector2(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
				boxCollider->activeOffset = Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName));
				boxCollider->b_isContinuous = CheckJsonBool(componentJson, "_isContinuous", objectName);
				boxCollider->b_isStatic = CheckJsonBool(componentJson, "_isStatic", objectName);
				boxCollider->b_isSolid = CheckJsonBool(componentJson, "_isSolid", objectName);
				boxCollider->activeLayer = CheckJsonInt(componentJson, "activeLayer", objectName);	
				boxCollider->b_showActiveRadius = CheckJsonBool(componentJson, "_showActiveRadius", objectName);
				boxCollider->b_isComposite = CheckJsonBool(componentJson, "_isComposite", objectName);
				boxCollider->rotation = objectRotation;

				prefab.components.emplace(componentID, boxCollider);
			}
			else if (type == "CircleCollider")
			{
				//CircleCollider* newCircleCollider = loadedObject.AddCircleCollider(id, _isActive, _isCollapsed);
				//newCircleCollider->SetActiveRadiusGrid(CheckJsonFloat(componentJson, "activeRadius", objectName));
				//newCircleCollider->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
				//newCircleCollider->SetIsContinuous(CheckJsonBool(componentJson, "_isContinuous", objectName));
				//newCircleCollider->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
				//newCircleCollider->SetIsSolid(CheckJsonBool(componentJson, "_isSolid", objectName));
				//newCircleCollider->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
				//newCircleCollider->SetIsComposite(CheckJsonBool(componentJson, "_isComposite", objectName));
			}
			else if (type == "RigidBody")
			{
				//RigidBody* newRigidBody = loadedObject.AddRigidBody(id, _isActive, _isCollapsed);
				//newRigidBody->SetMass(CheckJsonFloat(componentJson, "mass", objectName));
				//newRigidBody->SetAngularDrag(CheckJsonFloat(componentJson, "angularDrag", objectName));
				//newRigidBody->SetGravity(CheckJsonFloat(componentJson, "gravity", objectName));
				//newRigidBody->SetFallingGravity(CheckJsonFloat(componentJson, "fallingGravity", objectName));
				//newRigidBody->SetFriction(CheckJsonFloat(componentJson, "friction", objectName));
				//newRigidBody->SetWindResistance(CheckJsonFloat(componentJson, "windResistance", objectName));
				//newRigidBody->SetEquilibriumForce(CheckJsonFloat(componentJson, "equilibriumForce", objectName));
				//newRigidBody->SetTerminalVelocity(CheckJsonFloat(componentJson, "terminalVelocity", objectName));
				//newRigidBody->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
				//newRigidBody->SetTorquesAllowed(CheckJsonBool(componentJson, "_allowTorques", objectName));
			}
			else if (type == "TileMap")
			{
				//TileMap* newTileMap = loadedObject.AddTileMap(id, _isActive, _isCollapsed);
				//newTileMap->SetWidth(CheckJsonInt(componentJson, "width", objectName));
				//newTileMap->SetHeight(CheckJsonInt(componentJson, "height", objectName));
				//newTileMap->SetTileWidth(CheckJsonInt(componentJson, "tileWidth", objectName));
				//newTileMap->SetTileHeight(CheckJsonInt(componentJson, "tileHeight", objectName));

				//// Get used TileSet names
				//if (JsonContains(componentJson, "tileSets", objectName))
				//{
				//	for (int tileSet = 0; tileSet < componentJson["tileSets"].size(); tileSet++)
				//	{
				//		json tileSetJson = componentJson["tileSets"][tileSet];
				//		newTileMap->AddTileSet(CheckJsonString(tileSetJson, "name", objectName));
				//	}
				//}
				//// Get Tile data
				//if (JsonContains(componentJson, "tiles", objectName))
				//{
				//	for (int tile = 0; tile < componentJson["tiles"].size(); tile++)
				//	{
				//		json tileJson = componentJson["tiles"][tile];
				//		float x = CheckJsonFloat(tileJson, "tileCoordX", objectName);
				//		float y = CheckJsonFloat(tileJson, "tileCoordY", objectName);
				//		std::string tileSetName = CheckJsonString(tileJson, "tileSetName", objectName);
				//		int tileSetIndex = CheckJsonInt(tileJson, "tileSetIndex", objectName);

				//		if (tileSetName != "" && tileSetIndex != -1)
				//		{
				//			newTileMap->SetTile(Vector2(x, y), GetTileSet(tileSetName), tileSetIndex);
				//		}
				//	}
				//}
				//// Get Collision Area data
				//if (JsonContains(componentJson, "collisionAreas", objectName))
				//{
				//	for (int collisionArea = 0; collisionArea < componentJson["collisionAreas"].size(); collisionArea++)
				//	{
				//		json colliderAreaJson = componentJson["collisionAreas"][collisionArea];
				//		json colliderDataJson = colliderAreaJson["areaData"];
				//		std::string collisionAreaName = CheckJsonString(colliderAreaJson, "name", objectName);
				//		std::vector<std::pair<Vector2, Vector2>> collisionCoordBuffer;

				//		for (int colArea = 0; colArea < colliderDataJson.size(); colArea++)
				//		{
				//			Vector2 startCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "startCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "startCoordY", objectName));
				//			Vector2 endCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "endCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "endCoordY", objectName));
				//			std::pair<Vector2, Vector2> colPair = { startCoord, endCoord };
				//			collisionCoordBuffer.push_back(colPair);
				//		}

				//		if (collisionCoordBuffer.size() > 0)
				//		{
				//			newTileMap->SetCollisionAreaValues(collisionAreaName, collisionCoordBuffer);
				//		}
				//	}
				//}
			}


		}

		// Save copy of the root object
		prefab.objects.emplace(object.ID, object);
		if (object.parentID == -1)
		{
			prefab.rootObject = object;
		}
	}

	void PrefabManager::CreatePrefab(std::string path, GameObject gameObject)
	{
		std::string prefabName = FlatEngine::GetFilenameFromPath(path);

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our gameObject json objects
		json prefabObjectJsonArray;

		gameObject.SetName(prefabName);
		gameObject.SetIsPrefab(false);
		gameObject.SetPrefabName("");
		gameObject.SetPrefabSpawnLocation(Vector2(0, 0));

		// Push the parent
		prefabObjectJsonArray.push_back(CreateJsonFromObject(gameObject));

		// Push the children
		std::vector<long> childIDs = gameObject.GetChildren();
		for (int i = 0; i < childIDs.size(); i++)
		{
			// Add the gameObject json to the prefabObjectJsonArray
			prefabObjectJsonArray.push_back(CreateJsonFromObject(*GetObjectById(childIDs[i])));
		}

		// Recreate the GameObjects json object and add the array as the content
		json prefabObject = json::object({ { "Prefab", prefabObjectJsonArray }, { "Name", prefabName } });

		// Add the GameObjects object contents to the file
		file_obj << prefabObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();

		// Reinitialize prefabs
		InitializePrefabs();
	}

	void PrefabManager::InitializePrefabs()
	{
		m_prefabs.clear();
		std::vector<std::string> prefabPaths = FindAllFilesWithExtension(GetDir("projectDir"), ".prf");

		for (std::string path : prefabPaths)
		{
			std::filesystem::path prefabPath(path);
			
			Prefab prefab;
			prefab.name = GetFilenameFromPath(path);

			json prefabJson = LoadFileData(prefabPath.string());
			if (prefabJson != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto prefabObjects = prefabJson["Prefab"];
											
				if (prefabObjects != "NULL")
				{					
					for (int i = 0; i < prefabObjects.size(); i++)
					{					
						CreatePrefabFromJson(prefabObjects[i], prefab);
					}
				}

				// Add pair to m_prefabs
				m_prefabs.emplace(prefabJson["Name"], prefab);
			}
		}
	}

	GameObject *PrefabManager::Instantiate(std::string prefabName, Vector2 position, long parentID, long ID)
	{
		GameObject* instantiatedObject = nullptr;

		if (m_prefabs.count(prefabName) > 0)
		{
			Prefab prefab = m_prefabs.at(prefabName);
			GameObjectPrefabData root = prefab.rootObject;

			// Root object
			instantiatedObject = CreateGameObject(parentID, ID);

			if (ID != -1)
			{
				instantiatedObject->SetID(ID);
			}

			instantiatedObject->SetName(prefabName);
			instantiatedObject->SetIsPrefab(true);
			instantiatedObject->SetPrefabName(prefabName);
			instantiatedObject->SetPrefabSpawnLocation(position);
			instantiatedObject->SetActive(root.b_isActive);
			instantiatedObject->SetTagList(root.tagList);

			for (long componentID : root.componentIDs)
			{
				if (prefab.components.at(componentID)->type == "Transform")
				{
					std::shared_ptr<TransformPrefabData> transformData = std::static_pointer_cast<TransformPrefabData>(prefab.components.at(componentID));
					Transform* transform = instantiatedObject->GetTransform();
					transform->SetActive(transformData->b_isActive);
					transform->SetCollapsed(transformData->b_isCollapsed);
					transform->SetScale(transformData->scale);
					transform->SetRotation(transformData->rotation);
					transform->SetOrigin(transformData->origin);
					if (parentID != -1)
					{
						transform->SetInitialPosition(transformData->position);
					}
					else
					{
						transform->SetInitialPosition(position);
					}
				}
				else if (prefab.components.at(componentID)->type == "Sprite")
				{
					std::shared_ptr<SpritePrefabData> spriteData = std::static_pointer_cast<SpritePrefabData>(prefab.components.at(componentID));
					Sprite* sprite = instantiatedObject->AddSprite(-1, spriteData->b_isActive, spriteData->b_isCollapsed);
					sprite->SetTexture(spriteData->path);
					sprite->SetOffset(spriteData->offset);
					sprite->SetPivotPoint(spriteData->pivotPoint);
					sprite->SetRenderOrder(spriteData->renderOrder);
					sprite->SetTintColor(spriteData->tintColor);
				}
				else if (prefab.components.at(componentID)->type == "Sprite")
				{
					std::shared_ptr<SpritePrefabData> spriteData = std::static_pointer_cast<SpritePrefabData>(prefab.components.at(componentID));
					Sprite* sprite = instantiatedObject->AddSprite(-1, spriteData->b_isActive, spriteData->b_isCollapsed);
					sprite->SetTexture(spriteData->path);
					sprite->SetOffset(spriteData->offset);
					sprite->SetPivotPoint(spriteData->pivotPoint);
					sprite->SetRenderOrder(spriteData->renderOrder);
					sprite->SetTintColor(spriteData->tintColor);
				}
			}
		}

		return instantiatedObject;
	}

	std::map<std::string, Prefab> PrefabManager::GetPrefabs()
	{
		return m_prefabs;
	}
}