#include "GameBoard.h"
#include "scripts/King.h"
#include "scripts/Queen.h"
#include "scripts/Rook.h"
#include "scripts/Bishop.h"
#include "scripts/Knight.h"
#include "scripts/Pawn.h"
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
	int colCounter = 0;
	int rowCounter = 0;

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

				if (colCounter == 7)
				{
					rowCounter++;
					colCounter = 0;
				}
				else
					colCounter++;
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
	FlatEngine::LogFloat(movingTo->column, "To Column: ");
	FlatEngine::LogFloat(movingTo->row, "To Row: ");
	// Deactivate the button
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(movingFrom->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(false);

}

void GameBoard::MovePiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> movingFrom)
{
	movingTo->RemovePiece();
	movingTo->AssignPiece(movingFrom->pieceName, movingFrom->spritePath, movingFrom->pieceColor, movingFrom->spriteYOffset);
	movingFrom->RemovePiece();
}


void GameBoard::SetupBoard()
{
	for (int i = 0; i < boardSquares.size(); i++)
	{
		std::shared_ptr<BoardSquare> square = boardSquares[i];

		// BLACK PIECES
		if (square->column == 7)
		{
			if (square->row == 0)
			{
				square->AssignPiece("Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
			else if (square->row == 1)
			{
				square->AssignPiece("Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			else if (square->row == 2)
			{
				square->AssignPiece("Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			else if (square->row == 3)
			{
				square->AssignPiece("Queen_B", "assets/images/pieces/queenB.png", "black", 29);
			}
			else if (square->row == 4)
			{
				square->AssignPiece("King_B", "assets/images/pieces/kingB.png", "black", 29);
			}
			else if (square->row == 5)
			{
				square->AssignPiece("Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			else if (square->row == 6)
			{
				square->AssignPiece("Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			else if (square->row == 7)
			{
				square->AssignPiece("Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
		}
		else if (square->column == 6)
		{
			square->AssignPiece("Pawn_B", "assets/images/pieces/pawnB.png", "black", 20);
		}

		// WHITE PIECES
		else if (square->column == 1)
		{
			square->AssignPiece("Pawn_B", "assets/images/pieces/pawnW.png", "white", 20);
		}
		else if (square->column == 0)
		{
			if (square->row == 0)
			{
				square->AssignPiece("Rook_W", "assets/images/pieces/rookW.png", "white", 21);
			}
			else if (square->row == 1)
			{
				square->AssignPiece("Knight_W", "assets/images/pieces/knightW.png", "white", 25);
			}
			else if (square->row == 2)
			{
				square->AssignPiece("Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			else if (square->row == 3)
			{
				square->AssignPiece("Queen_W", "assets/images/pieces/queenW.png", "white", 29);
			}
			else if (square->row == 4)
			{
				square->AssignPiece("King_W", "assets/images/pieces/kingW.png", "white", 29);
			}
			else if (square->row == 5)
			{
				square->AssignPiece("Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			else if (square->row == 6)
			{
				square->AssignPiece("Knight_W", "assets/images/pieces/knightW.png", "white", 25);
			}
			else if (square->row == 7)
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


