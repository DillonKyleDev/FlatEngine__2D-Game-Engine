#pragma once
#include <string>
#include <memory>
#include "GameObject.h"

namespace FlatEngine
{
	class Scene;

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		std::shared_ptr<Scene> CreateNewScene();
		void SaveScene(std::shared_ptr<Scene> scene, std::string filename);
		void SaveCurrentScene();
		void LoadScene(std::string name);
		static json CreateJsonFromObject(std::shared_ptr<GameObject> gameObject);
		static std::shared_ptr<GameObject>CreateObjectFromJson(json objectJson, std::shared_ptr<Scene> loadedScene = nullptr);
		std::shared_ptr<Scene> GetLoadedScene();
		std::string GetLoadedScenePath();
		void SaveAnimationPreviewObjects();
		void LoadAnimationPreviewObjects();

	private:
		std::shared_ptr<Scene> loadedScene;
		std::string loadedScenePath;
		std::vector<std::shared_ptr<GameObject>> animatorPreviewObjects;
	};
}

