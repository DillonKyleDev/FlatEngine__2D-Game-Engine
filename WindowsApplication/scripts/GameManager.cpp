#include "GameManager.h"
#include "../FlatEngine.h"
#include "../ScriptComponent.h"
#include "../Sprite.h"
#include "../Transform.h"


GameManager::GameManager(long ownerID) : GameScript(ownerID)
{
	this->SetName("GameManager");
	_paused = false;
	pauseScreen = nullptr;
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	//pauseScreen = FlatEngine::GetObjectByName("PauseScreen");
}

void GameManager::Update(float deltaTime)
{
}

void GameManager::PauseGame()
{
	_paused = !_paused;
	//pauseScreen->SetActive(_paused);
}