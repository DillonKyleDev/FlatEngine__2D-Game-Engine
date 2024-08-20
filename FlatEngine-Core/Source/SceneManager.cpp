#include "SceneManager.h"
#include "Scene.h"
#include "Button.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Canvas.h"
#include "Script.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "TagList.h"

#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>


namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		m_loadedScene = Scene();
		m_loadedScenePath = "";
		m_animatorPreviewObjects = std::vector<GameObject*>();
	}

	SceneManager::~SceneManager()
	{
	}

	Scene *SceneManager::CreateNewScene()
	{
		m_loadedScene = Scene();
		return &m_loadedScene;
	}

	void SceneManager::SaveScene(Scene *scene, std::string filepath)
	{
		m_loadedScenePath = filepath;

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filepath);

		// Delete old contents of the file
		file_obj.open(filepath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(filepath, std::ios::app);
 
		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;

		std::map<long, GameObject> &sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
			{
				// Finally, add the gameObject json to the sceneObjectsJsonArray
				sceneObjectsJsonArray.push_back(CreateJsonFromObject(iter->second));
				iter++;
			}
		}
		else
		{
			sceneObjectsJsonArray.push_back("NULL");
		}

		// Recreate the GameObjects json object and add the array as the content
		json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SceneManager::SaveCurrentScene()
	{
		SaveScene(&m_loadedScene, m_loadedScenePath);
	}

	bool SceneManager::LoadScene(std::string filepath)
	{
		std::string fileName = GetFilenameFromPath(filepath, false);
		bool b_success = true;

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filepath);

		// Open file in in mode
		file_obj.open(filepath, std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj.good())
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				if (line != "")
					fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		if (file_obj.good() && fileContent != "")
		{
			// Start up a new scene
			m_loadedScene = Scene();
			m_loadedScenePath = filepath;
			m_loadedScene.SetName(fileName);
			m_loadedScene.SetPath(filepath);

			// Go from string to json object
			json fileContentJson = json::parse(fileContent);

			if (fileContentJson.contains("Scene GameObjects") && fileContentJson["Scene GameObjects"][0] != "NULL")
			{
				auto firstObjectName = fileContentJson["Scene GameObjects"];

				//Getting data from the json 
				//std::string name = firstObjectName[0]["name"];

				// Loop through the saved GameObjects in the JSON file
				for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
				{
					// Add created GameObject to our freshScene
					GameObject loadedObject = CreateObjectFromJson(fileContentJson["Scene GameObjects"][i]);
					// Check for primary camera
					if (loadedObject.HasComponent("Camera") && loadedObject.GetCamera()->IsPrimary())
						m_loadedScene.SetPrimaryCamera(loadedObject.GetCamera());
					// If loaded object was a prefab, it will have been Instantiated, which already adds the object to the loaded scene
					if (!loadedObject.IsPrefab())
						m_loadedScene.AddSceneObject(loadedObject);
				}

				F_Application->OnLoadScene(fileName);
			}
		}
		else
		{
			FlatEngine::LogString("Failed to load scene: " + fileName);
			b_success = false;
		}

		return b_success;
	}

	Scene *SceneManager::GetLoadedScene()
	{
		return &m_loadedScene;
	}
	
	std::string SceneManager::GetLoadedScenePath()
	{
		return m_loadedScenePath;
	}
	
	void SceneManager::SaveAnimationPreviewObjects()
	{
		if (m_loadedScene.GetAnimatorPreviewObjects().size() > 0)
			m_animatorPreviewObjects = m_loadedScene.GetAnimatorPreviewObjects();
	}
	
	void SceneManager::LoadAnimationPreviewObjects()
	{
		m_loadedScene.SetAnimatorPreviewObjects(m_animatorPreviewObjects);
	}
}