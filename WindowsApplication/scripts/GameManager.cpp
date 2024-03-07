#include "GameManager.h"
#include "../FlatEngine.h"
#include "../ScriptComponent.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


GameManager::GameManager()
{
	this->playerTurn = "white";
	this->selectedPiece = nullptr;
	// Get the GameBoard script in the scene
	std::shared_ptr<FlatEngine::GameObject> boardObject = FlatEngine::GetObjectByName("GameBoard");
	std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(boardObject->GetComponent(FlatEngine::ComponentTypes::Script));
	this->gameBoard = std::static_pointer_cast<GameBoard>(scriptComponent->GetScriptInstance());
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

void GameManager::SetWhiteActive(bool _isActive)
{
	
}

void GameManager::SelectPiece(std::shared_ptr<Piece> piece)
{
	if (piece->GetColor() == this->playerTurn)
	{

	}
}

void GameManager::CheckAvailableMoves()
{

}