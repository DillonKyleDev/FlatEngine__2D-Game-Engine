#include "GameBoard.h"
#include "../FlatEngine.h"


GameBoard::GameBoard()
{
	this->boardSquares = std::vector<std::shared_ptr<BoardSquare>>();
	this->squareIDs = std::vector<long>();
}

GameBoard::~GameBoard()
{
	this->SetName("GameBoard");
}

void GameBoard::Awake()
{
	// Create all board squares and associate them with a square transform in engine
	std::vector<long> childrenIDs = this->GetOwner()->GetChildren();
	int colCounter = 1;
	int rowCounter = 1;

	// Collect all in-engine board squares
	for (long childID : childrenIDs)
	{
		std::shared_ptr<FlatEngine::GameObject> child = FlatEngine::GetObjectById(childID);

		if (child->GetName() == "Positions")
		{
			std::vector<long> positions = child->GetChildren();

			for (long positionID : positions)
			{
				// a1, a2, a3, a4, etc...
				std::shared_ptr<FlatEngine::GameObject> positionObject = FlatEngine::GetObjectById(positionID);

				// Grab the BoardSquare script component off the gameboard square object
				std::shared_ptr<FlatEngine::Component> component = positionObject->GetComponent(FlatEngine::ComponentTypes::Script);
				std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(component);
				std::shared_ptr<BoardSquare> boardSquare = std::static_pointer_cast<BoardSquare>(scriptComponent->GetScriptInstance());
				this->squareIDs.push_back(positionID);

				// Add a button to the gameboard square object and set dimensions
				std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(positionObject->AddComponent(FlatEngine::ComponentTypes::Button));
				button->SetActiveDimensions(7, 7);
				button->SetActive(false);
				boardSquare->SetOwner(positionObject);

				// Add a child to each board square location for piece sprite rendering
				std::shared_ptr<FlatEngine::GameObject> pieceChild = FlatEngine::CreateGameObject(positionObject->GetID());
				positionObject->AddChild(pieceChild->GetID());
				pieceChild->SetName("piece sprite");
				pieceChild->AddComponent(FlatEngine::ComponentTypes::Transform);
				pieceChild->AddComponent(FlatEngine::ComponentTypes::Sprite);
				std::shared_ptr<FlatEngine::Sprite> pieceSprite = pieceChild->GetSpriteComponent();
				pieceSprite->SetTexture("assets/images/pieces/Clear.png");
				pieceSprite->SetRenderOrder(5);
				
				// Add a sprite to each board square object for selected effects and highlighting
				std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(positionObject->AddComponent(FlatEngine::ComponentTypes::Sprite));

				boardSquare->column = colCounter;
				boardSquare->row = rowCounter;

				this->boardSquares.push_back(boardSquare);

				if (rowCounter == 8)
				{
					colCounter++;
					rowCounter = 1;
				}
				else
					rowCounter++;
			}
		}
	}
}

void GameBoard::Start()
{
}

void GameBoard::Update(float deltaTime)
{
}


void GameBoard::TakePiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> movingFrom)
{
	movingTo->RemovePiece();
	movingTo->AssignPiece(movingFrom->pieceName, movingFrom->spritePath, movingFrom->pieceColor, movingFrom->spriteYOffset);
	movingFrom->RemovePiece();
	// Deactivate the button
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(movingFrom->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(false);
}

void GameBoard::MovePiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> movingFrom)
{
	movingTo->AssignPiece(movingFrom->pieceName, movingFrom->spritePath, movingFrom->pieceColor, movingFrom->spriteYOffset);
	movingFrom->RemovePiece();
	// Deactivate the button
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(movingFrom->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(false);
}

void GameBoard::SpawnPiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> peiceToSpawn)
{
	if (movingTo->pieceName != "")
		movingTo->RemovePiece();
	movingTo->AssignPiece(peiceToSpawn->pieceName, peiceToSpawn->spritePath, peiceToSpawn->pieceColor, peiceToSpawn->spriteYOffset);
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(movingTo->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(true);
}


void GameBoard::SetupBoard()
{
	for (int i = 0; i < boardSquares.size(); i++)
	{
		std::shared_ptr<BoardSquare> square = boardSquares[i];

		// BLACK PIECES
		if (square->row == 8)
		{
			if (square->column == 1)
			{
				square->AssignPiece("Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
			else if (square->column == 2)
			{
				square->AssignPiece("Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			else if (square->column == 3)
			{
				square->AssignPiece("Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			else if (square->column == 4)
			{
				square->AssignPiece("Queen_B", "assets/images/pieces/queenB.png", "black", 29);
			}
			else if (square->column == 5)
			{
				square->AssignPiece("King_B", "assets/images/pieces/kingB.png", "black", 29);
			}
			else if (square->column == 6)
			{
				square->AssignPiece("Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			else if (square->column == 7)
			{
				square->AssignPiece("Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			else if (square->column == 8)
			{
				square->AssignPiece("Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
		}
		else if (square->row == 7)
		{
			square->AssignPiece("Pawn_B", "assets/images/pieces/pawnB.png", "black", 20);
		}

		// WHITE PIECES
		else if (square->row == 2)
		{
			square->AssignPiece("Pawn_W", "assets/images/pieces/pawnW.png", "white", 20);
		}
		else if (square->row == 1)
		{
			if (square->column == 1)
			{
				square->AssignPiece("Rook_W", "assets/images/pieces/rookW.png", "white", 21);
			}
			else if (square->column == 2)
			{
				square->AssignPiece("Knight_W", "assets/images/pieces/knightW.png", "white", 25);
			}
			else if (square->column == 3)
			{
				square->AssignPiece("Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			else if (square->column == 4)
			{
				square->AssignPiece("Queen_W", "assets/images/pieces/queenW.png", "white", 29);
			}
			else if (square->column == 5)
			{
				square->AssignPiece("King_W", "assets/images/pieces/kingW.png", "white", 29);
			}
			else if (square->column == 6)
			{
				square->AssignPiece("Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			else if (square->column == 7)
			{
				square->AssignPiece("Knight_W", "assets/images/pieces/knightW.png", "white", 25);
			}
			else if (square->column == 8)
			{	
				square->AssignPiece("Rook_W", "assets/images/pieces/rookW.png", "white", 21);
			}
		}
	}
}

std::vector<std::shared_ptr<BoardSquare>> GameBoard::GetBoardSquares()
{
	return this->boardSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameBoard::GetWhiteSquares()
{
	whiteSquares.clear();

	for (std::shared_ptr<BoardSquare> square : boardSquares)
	{
		if (square->pieceColor == "white")
			whiteSquares.push_back(square);
	}

	return whiteSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameBoard::GetBlackSquares()
{
	blackSquares.clear();

	for (std::shared_ptr<BoardSquare> square : boardSquares)
	{
		if (square->pieceColor == "black")
			blackSquares.push_back(square);
	}

	return blackSquares;
}


