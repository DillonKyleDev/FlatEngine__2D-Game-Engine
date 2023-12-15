#include "FlatEngine.h"
#include <imgui.h>
#include "GameObjectManager.h"


bool FlatEngine::RenderHierarchy()
{
	GameObject newGameObject;
	Component characterSprite;


	newGameObject.SetName("MainCharacter");
	newGameObject.AddComponent(characterSprite);
	std::string tempName = newGameObject.GetName();
	
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Game Object Manager");

	if (ImGui::Button("Create New GameObject"))
	{
		GameObjectManager::CreateGameObject("GameObjectTitle");
	}

	for (int i = 0; i < GameObjectManager::gameObjects.size(); i++)
	{
		ImGui::Text("GameObject");
	}

	ImGui::End();

	return true;
}