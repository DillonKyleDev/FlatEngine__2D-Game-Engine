#include "GameManager.h"
#include "../FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


GameManager::GameManager()
{

}

GameManager::~GameManager()
{

}

void GameManager::Start()
{
	FlatEngine::LogString("GameManager instantiated on: " + this->GetOwner()->GetName());
}

void GameManager::Update(float deltaTime)
{

}