#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "Vector2.h"


namespace FlatEngine 
{
	class GameObject;

	class PrefabManager
	{
	public:
		PrefabManager();
		~PrefabManager();
		void CreatePrefab(std::string path, std::shared_ptr<FlatEngine::GameObject> gameObject);
		void InitializePrefabs();
		std::shared_ptr<FlatEngine::GameObject> Instantiate(std::string prefabName, Vector2 position, long parentID = -1, long ID = -1);
		std::map<std::string, std::vector<std::shared_ptr<GameObject>>> GetPrefabs();

	private:
		std::map<std::string, std::vector<std::shared_ptr<FlatEngine::GameObject>>> m_prefabs;
	};
}
