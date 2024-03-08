#include "GameManager.h"
#include "../FlatEngine.h"
#include "../ScriptComponent.h"


GameManager::GameManager()
{
	this->SetName("GameManager");
	this->playerTurn = "white";
	this->selectedPiece = nullptr;
	this->selectedSquare = nullptr;
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
	this->gameBoard->SetupBoard();
}

void GameManager::Update(float deltaTime)
{
	// Check for win conditions
	// if no win conditions

	if (this->selectedPiece != nullptr)
	{
		
	}
	else
	{

	}

	
}

void GameManager::SetWhiteActive(bool _isActive)
{
	
}

void GameManager::SetSelectedPiece(std::shared_ptr<Piece> piece, std::shared_ptr<FlatEngine::GameObject> owner)
{
	// Deactivate all pieces that were activated
	///

	if (this->selectedPiece != piece)
	{
		// Deselect other piece if it was already selected
		if (this->selectedSquare != nullptr && this->selectedPiece != nullptr)
		{
			std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(this->selectedSquare->GetComponent(FlatEngine::ComponentTypes::Sprite));
			sprite->SetTexture("assets/images/pieces/Clear.png");

			std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(this->selectedPiece->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Transform));
			transform->SetPosition(Vector2(transform->GetPosition().x, transform->GetPosition().y - 0.1f));
		}

		// Add the newly selected piece and highlight its square
		this->selectedPiece = piece;
		this->selectedSquare = owner;
		std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(owner->GetComponent(FlatEngine::ComponentTypes::Sprite));
		sprite->SetTexture("assets/images/pieces/CanGo.png");

		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(this->selectedPiece->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Transform));
		transform->SetPosition(Vector2(transform->GetPosition().x, transform->GetPosition().y + 0.1f));
	}

	///
	if (piece->GetColor() == this->playerTurn)
	{

	}
}

void GameManager::CheckAvailableMoves()
{

}