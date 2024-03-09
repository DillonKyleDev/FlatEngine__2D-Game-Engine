#include "GameManager.h"
#include "../FlatEngine.h"
#include "../ScriptComponent.h"


GameManager::GameManager()
{
	this->SetName("GameManager");
	this->playerTurn = "white";
	this->selectedSquare = nullptr;
	// Get the GameBoard script in the scene
	std::shared_ptr<FlatEngine::GameObject> boardObject = FlatEngine::GetObjectByName("GameBoard");
	std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(boardObject->GetComponent(FlatEngine::ComponentTypes::Script));
	this->gameBoard = std::static_pointer_cast<GameBoard>(scriptComponent->GetScriptInstance());
	movePieceAudio = nullptr;
	this->_whitePiecesActive = false;
	this->_blackPiecesActive = false;
}

GameManager::~GameManager()
{

}

void GameManager::Start()
{
	FlatEngine::LogString("GameManager instantiated on: " + this->GetOwner()->GetName());
	this->gameBoard->SetupBoard();
	SetWhiteActive(true);
	SetBlackActive(false);
	movePieceAudio = std::static_pointer_cast<FlatEngine::Audio>(GetOwner()->GetComponent(FlatEngine::ComponentTypes::Audio));
	movePieceAudio->SetIsMusic(false);
}

void GameManager::Update(float deltaTime)
{
	// Check for win conditions
	// if no win conditions


	// Set correct piece buttons to active
	if (this->playerTurn == "white" && !this->_whitePiecesActive)
	{
		SetWhiteActive(true);
		SetBlackActive(false);
	}
	else if (this->playerTurn == "black" && !this->_blackPiecesActive)
	{
		SetWhiteActive(false);
		SetBlackActive(true);
	}
}

void GameManager::SetWhiteActive(bool _isActive)
{
	for (std::shared_ptr<BoardSquare> whiteSquare : this->gameBoard->GetWhiteSquares())
	{
		std::shared_ptr<FlatEngine::Button> whiteButton = std::static_pointer_cast<FlatEngine::Button>(whiteSquare->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		whiteButton->SetActive(_isActive);
	}
	this->_whitePiecesActive = _isActive;
}

void GameManager::SetBlackActive(bool _isActive)
{
	for (std::shared_ptr<BoardSquare> blackSquare : this->gameBoard->GetBlackSquares())
	{
		std::shared_ptr<FlatEngine::Button> blackButton = std::static_pointer_cast<FlatEngine::Button>(blackSquare->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		blackButton->SetActive(_isActive);
	}
	this->_blackPiecesActive = _isActive;
}

void GameManager::SetSelectedSquare(std::shared_ptr<BoardSquare> square)
{
	std::shared_ptr<FlatEngine::Sprite> newSprite = square->GetOwner()->GetSpriteComponent();
	std::shared_ptr<FlatEngine::Transform> newTransform = square->GetOwner()->GetFirstChild()->GetTransformComponent();
	std::shared_ptr<FlatEngine::Sprite> oldSprite = nullptr;
	std::shared_ptr<FlatEngine::Transform> oldTransform = nullptr;

	if (selectedSquare != nullptr)
	{
		oldSprite = selectedSquare->GetOwner()->GetSpriteComponent();
		oldTransform = selectedSquare->GetOwner()->GetFirstChild()->GetTransformComponent();
		// Deselect piece that was already selected
		oldSprite->SetTexture("assets/images/pieces/Clear.png");
		oldTransform->SetPosition(Vector2(oldTransform->GetPosition().x, oldTransform->GetPosition().y - 0.1f));
	}

	// All squares being selected should have valid actions

	// If no square has been clicked yet
	if (selectedSquare == nullptr)
	{
		selectedSquare = square;
		newSprite->SetTexture("assets/images/pieces/Selected.png");
		newTransform->SetPosition(Vector2(newTransform->GetPosition().x, newTransform->GetPosition().y + 0.1f));
	}
	// If clicking on the same square as before
	else if (selectedSquare == square)
	{
		selectedSquare = nullptr;
		DeactivateAvailableMoves();
	}
	// If square is the same color as the already selected square
	else if (square->pieceColor == selectedSquare->pieceColor)
	{
		// Select the new piece in its place
		selectedSquare = square;
		newSprite->SetTexture("assets/images/pieces/Selected.png");
		newTransform->SetPosition(Vector2(newTransform->GetPosition().x, newTransform->GetPosition().y + 0.1f));
	}
	// If the square is a piece that has a different color (even an empty square)
	else if (square->pieceColor != selectedSquare->pieceColor)
	{
		// Take the piece
		gameBoard->TakePiece(square, selectedSquare);
		DeactivateAvailableMoves();
		//movePieceAudio->Play();


		// Change turns
		this->selectedSquare = nullptr;
		if (this->playerTurn == "white")
			this->playerTurn = "black";
		else
			this->playerTurn = "white";
	}

	if (this->selectedSquare != nullptr)
	{
		DeactivateAvailableMoves();
		ActivateAvailableMoves();
	}
}

void GameManager::ActivateAvailableMoves()
{
	// If (selectedSquare->pieceName == "pawn")
	PawnMoves();
	// else if () {}....

	for (std::shared_ptr<BoardSquare> square : this->availableMoves)
	{
		// Change the sprite
		std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
		sprite->SetTexture("assets/images/pieces/CanMoveTo.png");
		// Activate the button
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		button->SetActive(true);
	}
}

void GameManager::DeactivateAvailableMoves()
{
	if (this->availableMoves.size() > 0)
	{
		for (std::shared_ptr<BoardSquare> square : this->availableMoves)
		{
			// Change the sprite
			std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
			sprite->SetTexture("assets/images/pieces/clear.png");
			// Deactivate the button
			std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
			button->SetActive(false);
		}
	}
}


// Sets available available spaces to that of a pawn
void GameManager::PawnMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (selectedSquare->pieceColor == "white" &&
			boardSquare->row == selectedSquare->row && boardSquare->column == selectedSquare->column + 1 &&
			(boardSquare->pieceName == "" || boardSquare->pieceColor != selectedSquare->pieceColor))
		{
			availableSquares.push_back(boardSquare);
		}
		else if (selectedSquare->pieceColor == "black" &&
			boardSquare->row == selectedSquare->row && boardSquare->column == selectedSquare->column - 1 &&
			(boardSquare->pieceName == "" || boardSquare->pieceColor != selectedSquare->pieceColor))
		{
			availableSquares.push_back(boardSquare);
		}
	}

	this->availableMoves = availableSquares;
}