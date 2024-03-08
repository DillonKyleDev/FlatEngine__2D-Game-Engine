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
	this->boardSquares = std::vector<std::vector<std::shared_ptr<BoardSquare>>>(8,std::vector<std::shared_ptr<BoardSquare>>(8));
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
			std::vector<long> grandChildrenIDs = child->GetChildren();

			// a, b, c, d, e, f, g, h
			for (long gChildID : grandChildrenIDs)
			{
				// a1, a2, a3, a4, etc...
				std::shared_ptr<FlatEngine::GameObject> gchild = FlatEngine::GetObjectById(gChildID);

				for (long ggchildID : gchild->GetChildren())
				{
					std::shared_ptr<FlatEngine::GameObject> ggchild = FlatEngine::GetObjectById(ggchildID);

					// Grab the BoardSquare script component off the gameboard square object
					std::shared_ptr<FlatEngine::Component> component = ggchild->GetComponent(FlatEngine::ComponentTypes::Script);
					std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(component);
					std::shared_ptr<BoardSquare> boardSquare = std::static_pointer_cast<BoardSquare>(scriptComponent->GetScriptInstance());
					this->squareIDs.push_back(ggchildID);

					// Add a button to the gameboard square object and set dimensions
					std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(ggchild->AddComponent(FlatEngine::ComponentTypes::Button));
					button->SetActiveDimensions(7, 7);
					button->SetActive(true);
					boardSquare->SetOwner(ggchild);
					
					// Add a sprite to each board square object for selected effects and highlighting
					std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(ggchild->AddComponent(FlatEngine::ComponentTypes::Sprite));

					this->boardSquares[colCounter][rowCounter] = boardSquare;

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
}

void GameBoard::Start()
{
}

void GameBoard::Update(float deltaTime)
{

}

void GameBoard::InitializeBoard()
{
}

void GameBoard::SetupPieces()
{
}

void GameBoard::CreatePiece(std::shared_ptr<FlatEngine::GameObject> gameObject, std::shared_ptr<BoardSquare> boardSquare, std::shared_ptr<Piece> piece, std::string name, std::string spritePath, std::string color, float yOffset)
{
	piece->SetOwner(piece->GetPieceObject());
	piece->SetName(name);
	piece->GetPieceObject()->SetName(name);
	piece->SetSprite(spritePath);
	piece->SetSpriteOffsetY(yOffset);
	piece->SetColor(color);
	boardSquare->SetPiece(piece);
	gameObject->AddChild(piece->GetPieceObject()->GetID());
}

void GameBoard::SetupBoard()
{
	int colCounter = 0;
	int rowCounter = 0;

	for (long squareID : this->squareIDs)
	{
		std::shared_ptr<FlatEngine::GameObject> squareObject = FlatEngine::GetObjectById(squareID);
		std::shared_ptr<BoardSquare> square = this->boardSquares[colCounter][rowCounter];

		// BLACK PIECES
		if (colCounter == 7)
		{
			if (rowCounter == 0)
			{
				std::shared_ptr<Rook> blackRook = std::make_shared<Rook>(squareObject->GetID());
				CreatePiece(squareObject, square, blackRook, "Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
			if (rowCounter == 1)
			{
				std::shared_ptr<Knight> blackKnight = std::make_shared<Knight>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackKnight, "Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			if (rowCounter == 2)
			{
				std::shared_ptr<Bishop> blackBishop = std::make_shared<Bishop>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackBishop, "Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			if (rowCounter == 3)
			{
				std::shared_ptr<Queen> blackQueen = std::make_shared<Queen>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackQueen, "Queen_B", "assets/images/pieces/queenB.png", "black", 29);
			}
			if (rowCounter == 4)
			{
				std::shared_ptr<King> blackKing = std::make_shared<King>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackKing, "King_B", "assets/images/pieces/kingB.png", "black", 29);
			}
			if (rowCounter == 5)
			{
				std::shared_ptr<Bishop> blackBishop = std::make_shared<Bishop>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackBishop, "Bishop_B", "assets/images/pieces/bishopB.png", "black", 27);
			}
			if (rowCounter == 6)
			{
				std::shared_ptr<Knight> blackKnight = std::make_shared<Knight>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, blackKnight, "Knight_B", "assets/images/pieces/knightB.png", "black", 25);
			}
			if (rowCounter == 7)
			{
				std::shared_ptr<Rook> blackRook = std::make_shared<Rook>(squareObject->GetID());
				CreatePiece(squareObject, square, blackRook, "Rook_B", "assets/images/pieces/rookB.png", "black", 21);
			}
		}
		else if (colCounter == 6)
		{
			std::shared_ptr<Pawn> blackPawn = std::make_shared<Pawn>(square->GetOwner()->GetID());
			CreatePiece(squareObject, square, blackPawn, "Pawn_B", "assets/images/pieces/pawnB.png", "black", 20);
		}

		// WHITE PIECES
		else if (colCounter == 1)
		{
			std::shared_ptr<Pawn> whitePawn = std::make_shared<Pawn>(square->GetOwner()->GetID());
			CreatePiece(squareObject, square, whitePawn, "Pawn_B", "assets/images/pieces/pawnW.png", "white", 20);
		}
		else if (colCounter == 0)
		{
			if (rowCounter == 0)
			{
				// White Rook
				std::shared_ptr<Rook> whiteRook = std::make_shared<Rook>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteRook, "Rook_W", "assets/images/pieces/rookW.png", "whitet", 21);
			}
			if (rowCounter == 1)
			{
				// White Knight
				std::shared_ptr<Knight> whiteKnight = std::make_shared<Knight>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteKnight, "Knight_W", "assets/images/pieces/knightW.png", "white", 25);

			}
			if (rowCounter == 2)
			{
				std::shared_ptr<Bishop> whiteBishop = std::make_shared<Bishop>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteBishop, "Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			if (rowCounter == 3)
			{
				std::shared_ptr<Queen> whiteQueen = std::make_shared<Queen>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteQueen, "Queen_W", "assets/images/pieces/queenW.png", "white", 29);
			}
			if (rowCounter == 4)
			{
				std::shared_ptr<King> whiteKing = std::make_shared<King>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteKing, "King_W", "assets/images/pieces/kingW.png", "white", 29);
			}
			if (rowCounter == 5)
			{
				std::shared_ptr<Bishop> whiteBishop = std::make_shared<Bishop>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteBishop, "Bishop_W", "assets/images/pieces/bishopW.png", "white", 27);
			}
			if (rowCounter == 6)
			{
				std::shared_ptr<Knight> whiteKnight = std::make_shared<Knight>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteKnight, "Knight_W", "assets/images/pieces/knightW.png", "white", 25);
			}
			if (rowCounter == 7)
			{	
				std::shared_ptr<Rook> whiteRook = std::make_shared<Rook>(square->GetOwner()->GetID());
				CreatePiece(squareObject, square, whiteRook, "Rook_W", "assets/images/pieces/rookW.png", "white", 21);
			}
		}

		if (colCounter == 7)
		{
			rowCounter++;
			colCounter = 0;
		}
		else
			colCounter++;
	}
}


