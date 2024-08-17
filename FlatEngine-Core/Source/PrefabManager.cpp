#include "PrefabManager.h"
#include "FlatEngine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"
#include "GameLoop.h"
#include <fstream>

namespace FlatEngine
{
	PrefabManager::PrefabManager()
	{
		m_prefabs = std::map<std::string, std::vector<FlatEngine::GameObject>>();
	}

	PrefabManager::~PrefabManager()
	{
	}

	void PrefabManager::CreatePrefab(std::string path, GameObject& gameObject)
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

		// Temporary fix.. Add support for map of long,GameObject instead of vector of GameObject
		std::vector<GameObject> viewObjects;
		for (std::map<long, GameObject>::iterator iter = GetSceneObjects().begin(); iter != GetSceneObjects().end();)
		{
			viewObjects.push_back(iter->second);
			iter++;
		}

		std::vector<GameObject> childrenVector = std::vector<GameObject>();
		GameObject copy = GameObject(&gameObject, childrenVector, viewObjects, -1);
		copy.SetName(prefabName);
		copy.SetIsPrefab(false);
		copy.SetPrefabName("");
		copy.SetPrefabSpawnLocation(Vector2(0, 0));

		// Push the parent
		prefabObjectJsonArray.push_back(CreateJsonFromObject(copy));

		// Push the children
		if (childrenVector.size() > 0)
		{
			for (int i = 0; i < childrenVector.size(); i++)
			{
				// Add the gameObject json to the prefabObjectJsonArray
				prefabObjectJsonArray.push_back(CreateJsonFromObject(childrenVector[i]));
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

		std::string path = "../runtime-assets/prefabs";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::vector<GameObject> prefabContainer;
			GameObject prefabObject = FlatEngine::GameObject();

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
						GameObject prefab = CreateObjectFromJson(prefabObjects[i]);
						prefabContainer.push_back(&prefab);
					}
				}

				// Add pair to m_prefabs
				m_prefabs.emplace(prefab["Name"], prefabContainer);
			}
		}
	}

	GameObject PrefabManager::Instantiate(std::string prefabName, Vector2 position, long parentID, long ID)
	{
		GameObject instantiatedObject;
		std::vector<GameObject> children = std::vector<GameObject>();

		for (std::pair<std::string, std::vector<GameObject>> pair : m_prefabs)
		{
			if (pair.first == prefabName)
			{
				for (GameObject prefabObject : pair.second)
				{
					if (prefabObject.GetName() == prefabName)
					{
						instantiatedObject = GameObject(&prefabObject, children, pair.second, parentID, ID);

						if (instantiatedObject.HasComponent("Transform"))
						{
							if (parentID == -1)
								instantiatedObject.GetTransform()->SetOrigin(Vector2(0, 0));
							instantiatedObject.GetTransform()->SetPosition(position);
						}

						// A little janky but there you go
						children.push_back(instantiatedObject);

						// Track instantiated object and children in the scene
						for (GameObject newObject : children)
							FlatEngine::GetLoadedScene()->AddSceneObject(newObject);

						FlatEngine::GetLoadedScene()->OnPrefabInstantiated(children);

						// Figure this out later so Prefabs stay up-to-date with their json files on reloading
						instantiatedObject.SetIsPrefab(true);
						instantiatedObject.SetPrefabName(prefabName);
						instantiatedObject.SetPrefabSpawnLocation(position);
					}
				}
			}
		}

		return instantiatedObject;
	}

	std::map<std::string, std::vector<GameObject>> &PrefabManager::GetPrefabs()
	{
		return m_prefabs;
	}
}