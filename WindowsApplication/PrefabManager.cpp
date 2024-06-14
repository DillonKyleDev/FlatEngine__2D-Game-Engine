#include "PrefabManager.h"
#include "FlatEngine.h"
#include <fstream>
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"


namespace FlatEngine
{
	PrefabManager::PrefabManager()
	{
		prefabs = std::vector<std::shared_ptr<FlatEngine::GameObject>>();
	}

	PrefabManager::~PrefabManager()
	{
	}

	void PrefabManager::CreatePrefab(std::string path, std::shared_ptr<FlatEngine::GameObject> gameObject)
	{
		std::string prefabName = FlatGui::GetFilenameFromPath(path);

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

		std::vector<std::shared_ptr<GameObject>> childrenVector = std::vector<std::shared_ptr<GameObject>>();
		std::shared_ptr<GameObject> copy = std::make_shared<GameObject>(gameObject, childrenVector, GetSceneObjects(), -1);
		copy->SetName(prefabName);

		// Push the parent
		prefabObjectJsonArray.push_back(SceneManager::CreateJsonFromObject(copy));

		// Push the children
		if (childrenVector.size() > 0)
		{
			for (int i = 0; i < childrenVector.size(); i++)
			{
				// Add the gameObject json to the prefabObjectJsonArray
				prefabObjectJsonArray.push_back(SceneManager::CreateJsonFromObject(childrenVector[i]));
			}
		}

		// Recreate the GameObjects json object and add the array as the content
		json prefabObject = json::object({ { "Prefab", prefabObjectJsonArray }, { "Name", prefabName } });

		// Add the GameObjects object contents to the file
		file_obj << prefabObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void PrefabManager::InitializePrefabs()
	{
		if (prefabs.size() > 0)
			prefabs.clear();

		std::string path = "C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\WindowsApplication\\prefabs";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::shared_ptr<FlatEngine::GameObject> newPrefab;

			// Create a new context to save the loaded keybindings to
			std::shared_ptr<FlatEngine::GameObject> prefabObject = std::make_shared<FlatEngine::GameObject>();

			json prefab = LoadFileData(entry.path().string());
			if (prefab != NULL)
			{
				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				auto prefabObjects = prefab["Prefab"];
											
				if (prefabObjects != "NULL")
				{					
					// Loop through the saved GameObjects in the JSON file
					for (int i = 0; i < prefabObjects.size(); i++)
					{
						// Add created GameObject to our freshScene
						prefabs.push_back(SceneManager::CreateObjectFromJson(prefabObjects[i]));
					}
				}
			}
		}
	}

	std::shared_ptr<FlatEngine::GameObject> PrefabManager::Instantiate(std::string prefabName, Vector2 position, long parentID)
	{
		std::shared_ptr<FlatEngine::GameObject> instantiatedObject = nullptr;
		std::vector<std::shared_ptr<GameObject>> children = std::vector<std::shared_ptr<GameObject>>();

		for (std::shared_ptr<GameObject> prefab : prefabs)
		{
			if (prefab->GetName() == prefabName)
			{
				instantiatedObject = std::make_shared<GameObject>(prefab, children, prefabs, parentID);

				if (instantiatedObject->HasComponent("Transform"))
				{
					if (parentID == -1)
						instantiatedObject->GetTransformComponent()->SetOrigin(Vector2(0,0));
					instantiatedObject->GetTransformComponent()->SetPosition(position);
				}
				

				// A little janky but there you go
				children.push_back(instantiatedObject);

				// Track instantiated object and children in the scene
				for (std::shared_ptr<GameObject> newObject : children)
					FlatEngine::GetLoadedScene()->AddSceneObject(newObject);

				FlatEngine::gameLoop->UpdateActiveColliders();
				FlatEngine::gameLoop->UpdateActiveRigidBodies();
				FlatEngine::gameLoop->InitializeScriptObjects(children);

				return instantiatedObject;
			}
		}

		return instantiatedObject;
	}

	std::shared_ptr<FlatEngine::GameObject> PrefabManager::CreateWallPrefab()
	{
		std::shared_ptr<FlatEngine::GameObject> newObject = CreateGameObject(-1);
		//newObject->AddComponent(ComponentTypes::Transform);
		//newObject->SetName("Wall(" + std::to_string(newObject->GetID()) + ")");
		//std::shared_ptr<Sprite> sprite = std::static_pointer_cast<Sprite>(newObject->AddComponent(ComponentTypes::Sprite));
		//sprite->SetTexture("assets/images/environment/light.png");
		//std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(newObject->AddComponent(ComponentTypes::BoxCollider));
		//boxCollider->SetIsStatic(true);
		//boxCollider->SetIsContinuous(false);
		//SetFocusedGameObjectID(newObject->GetID());

		return newObject;
	}

	std::shared_ptr<FlatEngine::GameObject> PrefabManager::CreateJumpPadPrefab()
	{
		std::shared_ptr<FlatEngine::GameObject> newObject = CreateGameObject(-1);
		//std::shared_ptr<Transform> transform = newObject->AddTransformComponent();
		//transform->SetScale(Vector2(1, 0.3f));
		//newObject->SetName("JumpPad(" + std::to_string(newObject->GetID()) + ")");
		//std::shared_ptr<Sprite> sprite = std::static_pointer_cast<Sprite>(newObject->AddComponent(ComponentTypes::Sprite));
		//sprite->SetTexture("assets/images/environment/tan.png");
		//std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(newObject->AddComponent(ComponentTypes::BoxCollider));
		//boxCollider->SetIsStatic(true);
		//boxCollider->SetIsSolid(false);
		//boxCollider->SetIsContinuous(false);
		//std::shared_ptr<ScriptComponent> script = newObject->AddScriptComponent();
		//script->SetAttachedScript("JumpPad");
		//SetFocusedGameObjectID(newObject->GetID());

		return newObject;
	}
}