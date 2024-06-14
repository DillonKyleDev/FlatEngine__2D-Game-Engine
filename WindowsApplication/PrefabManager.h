#pragma once
#include <string>
#include <memory>
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
		std::shared_ptr<FlatEngine::GameObject> Instantiate(std::string prefabName, Vector2 position, long parentID = -1);
		std::shared_ptr<FlatEngine::GameObject> CreateTransformPrefab();
		std::shared_ptr<FlatEngine::GameObject> CreateSpritePrefab();
		std::shared_ptr<FlatEngine::GameObject> CreateButtonPrefab();
		std::shared_ptr<FlatEngine::GameObject> CreateWallPrefab();
		std::shared_ptr<FlatEngine::GameObject> CreateJumpPadPrefab();

	private:
		std::vector<std::shared_ptr<FlatEngine::GameObject>> prefabs;
	};
}
