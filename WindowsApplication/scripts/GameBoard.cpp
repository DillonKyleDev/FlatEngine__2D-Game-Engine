#include "GameBoard.h"
#include "scripts/King.h"
#include "scripts/Queen.h"
#include "scripts/Rook.h"
#include "scripts/Bishop.h"
#include "scripts/Knight.h"
#include "scripts/Pawn.h"
#include "../FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// New the variable in GameLoop()::Start() function and delete it in GameLoop()::Stop();
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


GameBoard::GameBoard()
{
	//std::vector<std::shared_ptr<FlatEngine::GameObject>> columns(8);
	//std::vector<std::vector<std::shared_ptr<FlatEngine::GameObject>>> rows(8);
	this->boardSquares = std::vector<std::vector<std::shared_ptr<FlatEngine::GameObject>>>();
	this->squareIDs = std::vector<long>();
}

GameBoard::~GameBoard()
{

}

void GameBoard::Start()
{
	// Create all board squares and associate them with a square transform in engine
	std::vector<long> childrenIDs = this->GetOwner()->GetChildren();
	FlatEngine::LogString("Started GameBoard:");
	
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

				for (long ggChildID : gchild->GetChildren())
				{
					this->squareIDs.push_back(ggChildID);
				}
			}
		}
	}

	int colCounter = 0;
	int rowCounter = 0;

	for (long squareID : this->squareIDs)
	{
		std::shared_ptr<FlatEngine::GameObject> squareObject = FlatEngine::GetObjectById(squareID);
		std::vector<std::shared_ptr<FlatEngine::GameObject>> colVector;

		// Create the buttons on each square and set their dimensions
		std::shared_ptr<BoardSquare> square = std::make_shared<BoardSquare>();
		square->SetBoardLocation(squareObject);
		std::shared_ptr<FlatEngine::Button> squareButton = std::static_pointer_cast<FlatEngine::Button>(squareObject->AddComponent(FlatEngine::ComponentTypes::Button));
		squareButton->SetActiveDimensions(7, 7);
		squareButton->SetActive(false);

		// BLACK PIECES
		if (colCounter == 7)
		{
			if (rowCounter == 0)
			{
				// Black Rook
				std::shared_ptr<Rook> blackRook = std::make_shared<Rook>(square->GetBoardLocation()->GetID());
				blackRook->SetName("Rook_B");
				blackRook->SetSprite("assets/images/pieces/rookB.png");
				blackRook->SetSpriteOffsetY(21);
				blackRook->SetColor("black");
				square->SetPiece(blackRook);
				squareObject->AddChild(blackRook->GetPieceObject()->GetID());
			}
			if (rowCounter == 1)
			{
				// Black Knight
				std::shared_ptr<Knight> blackKnight = std::make_shared<Knight>(square->GetBoardLocation()->GetID());
				blackKnight->SetName("Knight_B");
				blackKnight->SetSprite("assets/images/pieces/knightB.png");
				blackKnight->SetSpriteOffsetY(25);
				blackKnight->SetColor("black");
				square->SetPiece(blackKnight);
				squareObject->AddChild(blackKnight->GetPieceObject()->GetID());
			}
			if (rowCounter == 2)
			{
				// Black Bishop
				std::shared_ptr<Bishop> blackBishop = std::make_shared<Bishop>(square->GetBoardLocation()->GetID());
				blackBishop->SetName("bishop_B");
				blackBishop->SetSprite("assets/images/pieces/bishopB.png");
				blackBishop->SetSpriteOffsetY(27);
				blackBishop->SetColor("black");
				square->SetPiece(blackBishop);
				squareObject->AddChild(blackBishop->GetPieceObject()->GetID());
			}
			if (rowCounter == 3)
			{
				// Black Queen
				std::shared_ptr<Queen> blackQueen = std::make_shared<Queen>(square->GetBoardLocation()->GetID());
				blackQueen->SetName("queen_B");
				blackQueen->SetSprite("assets/images/pieces/queenB.png");
				blackQueen->SetSpriteOffsetY(29);
				blackQueen->SetColor("black");
				square->SetPiece(blackQueen);
				squareObject->AddChild(blackQueen->GetPieceObject()->GetID());
			}
			if (rowCounter == 4)
			{
				// Black King
				std::shared_ptr<King> blackKing = std::make_shared<King>(square->GetBoardLocation()->GetID());
				blackKing->SetName("king_B");
				blackKing->SetSprite("assets/images/pieces/kingB.png");
				blackKing->SetSpriteOffsetY(29);
				blackKing->SetColor("black");
				square->SetPiece(blackKing);
				squareObject->AddChild(blackKing->GetPieceObject()->GetID());
			}
			if (rowCounter == 5)
			{
				// Black Bishop
				std::shared_ptr<Bishop> blackBishop = std::make_shared<Bishop>(square->GetBoardLocation()->GetID());
				blackBishop->SetName("bishop_B");
				blackBishop->SetSprite("assets/images/pieces/bishopB.png");
				blackBishop->SetSpriteOffsetY(27);
				blackBishop->SetColor("black");
				square->SetPiece(blackBishop);
				squareObject->AddChild(blackBishop->GetPieceObject()->GetID());
			}
			if (rowCounter == 6)
			{
				// Black Knight
				std::shared_ptr<Knight> blackKnight = std::make_shared<Knight>(square->GetBoardLocation()->GetID());
				blackKnight->SetName("Knight_B");
				blackKnight->SetSprite("assets/images/pieces/knightB.png");
				blackKnight->SetSpriteOffsetY(25);
				blackKnight->SetColor("black");
				square->SetPiece(blackKnight);
				squareObject->AddChild(blackKnight->GetPieceObject()->GetID());
			}
			if (rowCounter == 7)
			{
				// Black Rook	
				std::shared_ptr<Rook> blackRook = std::make_shared<Rook>(square->GetBoardLocation()->GetID());
				blackRook->SetName("Rook_B");
				blackRook->SetSprite("assets/images/pieces/rookB.png");
				blackRook->SetSpriteOffsetY(21);
				blackRook->SetColor("black");
				square->SetPiece(blackRook);
				squareObject->AddChild(blackRook->GetPieceObject()->GetID());
			}
		}
		else if (colCounter == 6)
		{
			std::shared_ptr<Pawn> blackPawn = std::make_shared<Pawn>(square->GetBoardLocation()->GetID());
			blackPawn->SetName("Pawn_B");
			blackPawn->SetSprite("assets/images/pieces/pawnB.png");
			blackPawn->SetSpriteOffsetY(20);
			blackPawn->SetColor("black");
			square->SetPiece(blackPawn);
			squareObject->AddChild(blackPawn->GetPieceObject()->GetID());
		}

		// WHITE PIECES
		else if (colCounter == 1)
		{
			std::shared_ptr<Pawn> whitePawn = std::make_shared<Pawn>(square->GetBoardLocation()->GetID());
			whitePawn->SetName("Pawn_W");
			whitePawn->SetSprite("assets/images/pieces/pawnW.png");
			whitePawn->SetSpriteOffsetY(20);
			whitePawn->SetColor("white");
			square->SetPiece(whitePawn);
			squareObject->AddChild(whitePawn->GetPieceObject()->GetID());
		}
		else if (colCounter == 0)
		{
			if (rowCounter == 0)
			{
				// White Rook
				std::shared_ptr<Rook> whiteRook = std::make_shared<Rook>(square->GetBoardLocation()->GetID());
				whiteRook->SetName("Rook_W");
				whiteRook->SetSprite("assets/images/pieces/rookW.png");
				whiteRook->SetSpriteOffsetY(21);
				whiteRook->SetColor("white");
				square->SetPiece(whiteRook);
				squareObject->AddChild(whiteRook->GetPieceObject()->GetID());
			}
			if (rowCounter == 1)
			{
				// White Knight
				std::shared_ptr<Knight> whiteKnight = std::make_shared<Knight>(square->GetBoardLocation()->GetID());
				whiteKnight->SetName("Knight_W");
				whiteKnight->SetSprite("assets/images/pieces/knightW.png");
				whiteKnight->SetSpriteOffsetY(25);
				whiteKnight->SetColor("white");
				square->SetPiece(whiteKnight);
				squareObject->AddChild(whiteKnight->GetPieceObject()->GetID());
			}
			if (rowCounter == 2)
			{
				// White Bishop
				std::shared_ptr<Bishop> whiteBishop = std::make_shared<Bishop>(square->GetBoardLocation()->GetID());
				whiteBishop->SetName("bishop_W");
				whiteBishop->SetSprite("assets/images/pieces/bishopW.png");
				whiteBishop->SetSpriteOffsetY(27);
				whiteBishop->SetColor("white");
				square->SetPiece(whiteBishop);
				squareObject->AddChild(whiteBishop->GetPieceObject()->GetID());
			}
			if (rowCounter == 3)
			{
				// White Queen
				std::shared_ptr<Queen> whiteQueen = std::make_shared<Queen>(square->GetBoardLocation()->GetID());
				whiteQueen->SetName("queen_W");
				whiteQueen->SetSprite("assets/images/pieces/queenW.png");
				whiteQueen->SetSpriteOffsetY(29);
				whiteQueen->SetColor("white");
				square->SetPiece(whiteQueen);
				squareObject->AddChild(whiteQueen->GetPieceObject()->GetID());
			}
			if (rowCounter == 4)
			{
				// White King
				std::shared_ptr<King> whiteKing = std::make_shared<King>(square->GetBoardLocation()->GetID());
				whiteKing->SetName("king_W");
				whiteKing->SetSprite("assets/images/pieces/kingW.png");
				whiteKing->SetSpriteOffsetY(29);
				whiteKing->SetColor("white");
				square->SetPiece(whiteKing);
				squareObject->AddChild(whiteKing->GetPieceObject()->GetID());
			}
			if (rowCounter == 5)
			{
				// White Bishop
				std::shared_ptr<Bishop> whiteBishop = std::make_shared<Bishop>(square->GetBoardLocation()->GetID());
				whiteBishop->SetName("bishop_W");
				whiteBishop->SetSprite("assets/images/pieces/bishopW.png");
				whiteBishop->SetSpriteOffsetY(27);
				whiteBishop->SetColor("white");
				square->SetPiece(whiteBishop);
				squareObject->AddChild(whiteBishop->GetPieceObject()->GetID());
			}
			if (rowCounter == 6)
			{
				// White Knight
				std::shared_ptr<Knight> whiteKnight = std::make_shared<Knight>(square->GetBoardLocation()->GetID());
				whiteKnight->SetName("Knight_W");
				whiteKnight->SetSprite("assets/images/pieces/knightW.png");
				whiteKnight->SetSpriteOffsetY(25);
				whiteKnight->SetColor("white");
				square->SetPiece(whiteKnight);
				squareObject->AddChild(whiteKnight->GetPieceObject()->GetID());
			}
			if (rowCounter == 7)
			{
				// White Rook	
				std::shared_ptr<Rook> whiteRook = std::make_shared<Rook>(square->GetBoardLocation()->GetID());
				whiteRook->SetName("Rook_W");
				whiteRook->SetSprite("assets/images/pieces/rookW.png");
				whiteRook->SetSpriteOffsetY(21);
				whiteRook->SetColor("white");
				square->SetPiece(whiteRook);
				squareObject->AddChild(whiteRook->GetPieceObject()->GetID());
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

void GameBoard::Update(float deltaTime)
{
	for (std::shared_ptr<FlatEngine::GameObject> entity : this->GetEntities())
	{
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(entity->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
	}
}


