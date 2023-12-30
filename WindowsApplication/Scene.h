#pragma once
#include <vector>
#include "GameObject.h"


namespace FlatEngine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void SetName(std::string name);
		std::string GetName();
		void AddSceneObject(GameObject *sceneObject);
		std::vector<GameObject*> GetSceneObjects();
		// Managing GameObjects
		void CreateGameObject();
		void DeleteGameObject(int sceneObjectIndex);
		void IncrementID();
		long GetCurrentID();

	private:
		std::string name;
		std::vector<GameObject*> sceneObjects;
		long currentID;
	};
}