#pragma once
#include <vector>
#include "GameObject.h"
#include "Camera.h"


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
		GameObject* GetObjectById(long ID);
		void CreateGameObject();
		void DeleteGameObject(int sceneObjectIndex);
		void IncrementID();
		long GetCurrentID();
		void SetPrimaryCamera(Camera* camera);
		void RemovePrimaryCamera();
		Camera* GetPrimaryCamera();

	private:
		std::string name;
		std::vector<GameObject*> sceneObjects;
		Camera* primaryCamera;
		long currentID;
	};
}