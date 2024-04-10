#pragma once
#include <string>
#include <memory>

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
		std::shared_ptr<Scene> GetLoadedScene();

	private:
		std::shared_ptr<Scene> loadedScene;
		std::string loadedScenePath;
	};
}

