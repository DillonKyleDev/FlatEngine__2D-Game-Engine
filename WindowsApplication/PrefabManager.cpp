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
		m_prefabs = std::map<std::string, std::vector<std::shared_ptr<FlatEngine::GameObject>>>();
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
		copy->SetIsPrefab(false);
		copy->SetPrefabName("");
		copy->SetPrefabSpawnLocation(Vector2(0, 0));

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

		// Reinitialize prefabs
		InitializePrefabs();
	}

	void PrefabManager::InitializePrefabs()
	{
		m_prefabs.clear();

		std::string path = "C:\\Users\\Dillon Kyle\\source\\repos\\FlatEngine\\WindowsApplication\\prefabs";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::vector<std::shared_ptr<GameObject>> prefabContainer;
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
						prefabContainer.push_back(SceneManager::CreateObjectFromJson(prefabObjects[i]));
					}
				}

				// Add pair to m_prefabs
				m_prefabs.emplace(prefab["Name"], prefabContainer);
			}
		}
	}

	std::shared_ptr<FlatEngine::GameObject> PrefabManager::Instantiate(std::string prefabName, Vector2 position, long parentID, long ID)
	{
		std::shared_ptr<FlatEngine::GameObject> instantiatedObject = nullptr;
		std::vector<std::shared_ptr<GameObject>> children = std::vector<std::shared_ptr<GameObject>>();

		for (std::pair<std::string, std::vector<std::shared_ptr<GameObject>>> pair : m_prefabs)
		{
			if (pair.first == prefabName)
			{
				for (std::shared_ptr<GameObject> prefabObject : pair.second)
				{
					if (prefabObject->GetName() == prefabName)
					{
						instantiatedObject = std::make_shared<GameObject>(prefabObject, children, pair.second, parentID, ID);

						if (instantiatedObject->HasComponent("Transform"))
						{
							if (parentID == -1)
								instantiatedObject->GetTransformComponent()->SetOrigin(Vector2(0, 0));
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

						// Figure this out later so Prefabs stay up-to-date with their json files on reloading
						instantiatedObject->SetIsPrefab(true);
						instantiatedObject->SetPrefabName(prefabName);
						instantiatedObject->SetPrefabSpawnLocation(position);
					}
				}
			}
		}

		return instantiatedObject;
	}

	std::map<std::string, std::vector<std::shared_ptr<GameObject>>> PrefabManager::GetPrefabs()
	{
		return m_prefabs;
	}
}