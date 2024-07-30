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
		void CreatePrefab(std::string path, GameObject& gameObject);
		void InitializePrefabs();
		GameObject Instantiate(std::string prefabName, Vector2 position, long parentID = -1, long ID = -1);
		std::map<std::string, std::vector<GameObject>> &GetPrefabs();

	private:
		std::map<std::string, std::vector<GameObject>> m_prefabs;
	};
}