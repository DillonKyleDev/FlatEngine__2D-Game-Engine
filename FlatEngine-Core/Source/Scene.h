#pragma once
#include <vector>
#include <string>
#include <memory>
#include <list>

namespace FlatEngine
{
	class GameObject;
	class Camera;

	class Scene
	{
		friend class SceneManager;
	public:
		Scene();
		~Scene();

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		GameObject AddSceneObject(GameObject sceneObject);
		std::vector<GameObject> &GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject> previewObjects);
		std::vector<GameObject> &GetAnimatorPreviewObjects();
		GameObject* GetObjectById(long ID);
		GameObject* GetObjectByName(std::string name);
		GameObject* CreateGameObject(long parentID = -1);
		void DeleteGameObject(long sceneObjectID);
		void DeleteChildrenAndSelf(GameObject objectToDelete); // Recursive
		void IncrementGameObjectID();
		long GetNextGameObjectID();
		void IncrementComponentID();
		long GetNextComponentID();
		void SetPrimaryCamera(Camera* camera);
		void RemovePrimaryCamera();
		Camera *GetPrimaryCamera();

	private:
		std::string name;
		std::string path;
		std::vector<GameObject> sceneObjects;	
		std::vector<GameObject> animatorPreviewObjects;
		Camera *primaryCamera;
		long nextGameObjectID;
		long nextComponentID;
	};
}