#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "ScriptComponent.h"
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
		if (sceneObjects.size() > 0)
		{
			for (int i = 0; i < sceneObjects.size(); i++)
			{
				// Declare components array json object for components
				json componentsArray = json::array();

				// Get the object we'll be working with and it's components
				GameObject* currentObject = scene->GetSceneObjects()[i];
				std::vector<FlatEngine::Component*> components = currentObject->GetComponents();

				for (int j = 0; j < components.size(); j++)
				{
					std::string typeString = components[j]->GetTypeString();

					if (typeString != "Null")
					{
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

		// Go from string to json object
		json fileContentJson = json::parse(fileContent);

		if (fileContentJson["Scene GameObjects"][0] != "NULL")
		{
			auto firstObjectName = fileContentJson["Scene GameObjects"];
			std::string name = firstObjectName[0]["name"];

			// Loop through the saved GameObjects in the JSON file
			for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
			{
				// Get data from the loaded object
				json currentObjectJson = fileContentJson["Scene GameObjects"][i];
				std::string loadedName = currentObjectJson["name"];

				// Create new GameObject to load the data into
				GameObject* loadedObject = new GameObject();
				loadedObject->SetName(loadedName);

				// Loop through the components in this GameObjects json
				for (int j = 0; j < currentObjectJson["components"].size(); j++)
				{
					std::string type = "";
					if (currentObjectJson["components"][j].contains("type"))
						type = currentObjectJson["components"][j]["type"];
					else
						FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'type' in object: ");

					//Add each loaded component to the newly created GameObject
					if (type == "Transform")
					{
						// Default values
						float xScale = 1;
						float yScale = 1;
						float xPos = 0;
						float yPos = 0;
						float rotation = 0;

						FlatEngine::Transform* newTransform = static_cast<FlatEngine::Transform*>(loadedObject->AddComponent(Component::ComponentTypes::Transform));
						// position
						if (currentObjectJson["components"][j].contains("xPos"))
							xPos = currentObjectJson["components"][j]["xPos"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'xPos' in object: ");

						if (currentObjectJson["components"][j].contains("yPos"))
							yPos = currentObjectJson["components"][j]["yPos"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'yPos' in object: ");

						// scale
						if (currentObjectJson["components"][j].contains("xScale"))
							xScale = currentObjectJson["components"][j]["xScale"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'xScale' in object: ");

						if (currentObjectJson["components"][j].contains("yScale"))
							yScale = currentObjectJson["components"][j]["yScale"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'yScale' in object: ");

						// rotation
						if (currentObjectJson["components"][j].contains("rotation"))
							rotation = currentObjectJson["components"][j]["rotation"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'rotation' in object: ");

						newTransform->SetPosition(Vector2(xPos, yPos));
						newTransform->SetScale(Vector2(xScale, yScale));
						newTransform->SetRotation(rotation);
					}
					else if (type == "Sprite")
					{
						FlatEngine::Sprite* newSprite = static_cast<FlatEngine::Sprite*>(loadedObject->AddComponent(Component::ComponentTypes::Sprite));

						// Default values
						std::string path = "";
						float xOffset = 0;
						float yOffset = 0;

						if (currentObjectJson["components"][j].contains("texture"))
							path = currentObjectJson["components"][j]["texture"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'texture' in object: ");
						if (currentObjectJson["components"][j].contains("xOffset"))
							xOffset = currentObjectJson["components"][j]["xOffset"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'xOffset' in object: ");
						if (currentObjectJson["components"][j].contains("yOffset"))
							yOffset = currentObjectJson["components"][j]["yOffset"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'yOffset' in object: ");

						newSprite->SetTexture(path);
						newSprite->SetOffset(Vector2(xOffset, yOffset));
					}
					else if (type == "Camera")
					{
						FlatEngine::Camera* newCamera = static_cast<FlatEngine::Camera*>(loadedObject->AddComponent(Component::ComponentTypes::Camera));

						// Default values
						float width = 400;
						float height = 300;
						bool _isPrimaryCamera = false;
						float zoom = 1;
						
						float f_red = 1;
						float f_green = 1;
						float f_blue = 1;
						float f_alpha = 1;

						// Width and height
						if (currentObjectJson["components"][j].contains("width"))
							width = currentObjectJson["components"][j]["width"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'width' in object: ");
						if (currentObjectJson["components"][j].contains("height"))
							height = currentObjectJson["components"][j]["height"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'height' in object: ");
						// Is primaryCamera
						if (currentObjectJson["components"][j].contains("_isPrimaryCamera"))
							_isPrimaryCamera = currentObjectJson["components"][j]["_isPrimaryCamera"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for '_isPrimaryCamera' in object: ");
						// Zoom
						if (currentObjectJson["components"][j].contains("zoom"))
							zoom = currentObjectJson["components"][j]["zoom"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'zoom' in object: ");
						// Frustrum RGBA values
						if (currentObjectJson["components"][j].contains("frustrumRed"))
							f_red = currentObjectJson["components"][j]["frustrumRed"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'frustrumRed' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumGreen"))
							f_green = currentObjectJson["components"][j]["frustrumGreen"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'frustrumGreen' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumBlue"))
							f_blue = currentObjectJson["components"][j]["frustrumBlue"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'frustrumBlue' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumAlpha"))
							f_alpha = currentObjectJson["components"][j]["frustrumAlpha"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'frustrumAlpha' in object: ");

						FlatEngine::LogFloat(f_red, "F-red: ");
						FlatEngine::LogFloat(f_green, "F-green: ");
						FlatEngine::LogFloat(f_blue, "f_blue: ");
						FlatEngine::LogFloat(f_alpha, "F-alpha: ");
						ImVec4 frustrumColor = ImVec4(f_red, f_green, f_blue, f_alpha);

						newCamera->SetDimensions(width, height);
						newCamera->SetPrimaryCamera(_isPrimaryCamera);
						newCamera->SetZoom(zoom);
						newCamera->SetFrustrumColor(frustrumColor);

						// If this camera is the primary camera, set it in the Scene as the primaryCamera
						if (_isPrimaryCamera)
							freshScene->SetPrimaryCamera(newCamera);
					}
					else if (type == "Script")
					{
						FlatEngine::ScriptComponent* newScript = static_cast<FlatEngine::ScriptComponent*>(loadedObject->AddComponent(Component::ComponentTypes::Script));

						// Default values
						std::string attachedScript = "";
						bool _isActive = false;

						if (currentObjectJson["components"][j].contains("attachedScript"))
							attachedScript = currentObjectJson["components"][j]["attachedScript"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for 'attachedScript' in object: ");

						if (currentObjectJson["components"][j].contains("_isActive"))
							_isActive = currentObjectJson["components"][j]["_isActive"];
						else
							FlatEngine::LogInt(j, "Saved scene json does not contain a value for '_isActive' in object: ");

						newScript->SetAttachedScript(attachedScript);
						newScript->SetActive(_isActive);
					}
				}

				// Add created GameObject to our freshScene
				freshScene->AddSceneObject(loadedObject);
			}
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