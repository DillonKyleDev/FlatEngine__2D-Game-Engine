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
		void AddSceneObject(GameObject sceneObject);
		void GetSceneObjects();

	private:
		std::string name;
		std::vector<GameObject> sceneObjects;
	};
}