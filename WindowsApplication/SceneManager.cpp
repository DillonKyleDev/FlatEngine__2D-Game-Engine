#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		this->loadedScene = new Scene();
	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::SaveScene(Scene *scene)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream("SavedScenes.json");

		// Open file in in mode
		file_obj.open("SavedScenes.json", std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj)
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		// Delete old contents of the file
		file_obj.open("SavedScenes.json", std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open("SavedScenes.json", std::ios::app);
 
		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;

		std::vector<GameObject*> sceneObjects = scene->GetSceneObjects();
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			// Declare components array json object for components
			json componentsArray = json::array();

			// Get the object we'll be working with and it's components
			GameObject *currentObject = scene->GetSceneObjects()[i];
			std::vector<FlatEngine::Component*> components = currentObject->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				std::string typeString = components[j]->GetTypeString();

				if (typeString == "Transform")
				{
					//FlatEngine::Transform *component = static_cast<FlatEngine::Transform*>(components[j]);
					std::string data = components[j]->GetData();
					componentsArray.push_back(json::parse(data));
				}
				else if (typeString == "Sprite")
				{
					//FlatEngine::Sprite *component = static_cast<FlatEngine::Sprite*>(components[j]);
					std::string data = components[j]->GetData();
					componentsArray.push_back(json::parse(data));
				}
			}

			std::string objectName = currentObject->GetName();

			json gameObjectJson = json::object({ {"name", objectName}, {"components", componentsArray} });

			// Adding elements to objects in this way doesn't seem to save correctly. ^ Use the way above ^
			//gameObjectJson += json::object_t::value_type("name", objectName);
			//gameObjectJson += json::object_t::value_type("components", componentsArray);

			// Finally, add the gameObject json to the sceneObjectsJsonArray
			sceneObjectsJsonArray.push_back(gameObjectJson);
		}

		// Recreate the GameObjects json object and add the array as the content
		json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SceneManager::LoadScene(std::string fileName)
	{
		delete this->loadedScene;
		this->loadedScene = nullptr;

		Scene *freshScene = new Scene();
		this->loadedScene = freshScene;

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream("SavedScenes.json");

		// Open file in in mode
		file_obj.open("SavedScenes.json", std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj)
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		fileContent = fileContent;

		// Go from string to json object
		json fileContentJson = json::parse(fileContent);
		fileContentJson = fileContentJson;

		auto firstObjectName = fileContentJson["Scene GameObjects"];
		firstObjectName = firstObjectName;

		std::string name = firstObjectName[0]["name"];
		name = name;

		// Loop through the saved GameObjects in the JSON file
		for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
		{
			// Get data from the loaded object
			json currentObjectJson = fileContentJson["Scene GameObjects"][i];
			std::string loadedName = currentObjectJson["name"];

			// Create new GameObject to load the data into
			GameObject *loadedObject = new GameObject();
			loadedObject->SetName(loadedName);

			// Loop through the components in this GameObjects json
			for (int j = 0; j < currentObjectJson["components"].size(); j++)
			{
				auto type = currentObjectJson["components"][j]["type"];

				//Add each loaded component to the newly created GameObject
				if (type == "Transform")
				{
					FlatEngine::Transform* newTransform = static_cast<FlatEngine::Transform*>(loadedObject->AddComponent(Component::ComponentTypes::Transform));
					float xPos = currentObjectJson["components"][j]["xPos"];
					float yPos = currentObjectJson["components"][j]["yPos"];
					float rotation = currentObjectJson["components"][j]["rotation"];

					newTransform->SetPosition(Vector2(xPos, yPos));
					newTransform->SetRotation(rotation);
				}
				else if (type == "Sprite")
				{
					FlatEngine::Sprite* newSprite = static_cast<FlatEngine::Sprite*>(loadedObject->AddComponent(Component::ComponentTypes::Sprite));
					std::string path = currentObjectJson["components"][j]["texture"];

					newSprite->SetTexture(path);
				}
			}

			// Add created GameObject to our freshScene
			freshScene->AddSceneObject(loadedObject);
		}

		// Assign our freshScene to the SceneManagers currently loadedScene member variable
		this->loadedScene = freshScene;
	}

	Scene *SceneManager::GetLoadedScene()
	{
		return this->loadedScene;
	}
}


// Alternative file writing method
//file_obj.write(fileContent.c_str(), fileContent.size());