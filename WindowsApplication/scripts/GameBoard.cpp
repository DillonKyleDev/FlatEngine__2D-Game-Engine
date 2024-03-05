#include "GameBoard.h"
#include "../FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// New the variable in GameLoop()::Start() function and delete it in GameLoop()::Stop();
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


GameBoard::GameBoard()
{
	squares = std::map<std::string, BoardSquare>();
	initialPositions = {
		{ "a1", "w_rook" },
		{ "a2", "w_pawn" },
		{ "a3", "empty" },
		{ "a4", "empty" },
		{ "a5", "empty" },
		{ "a6", "empty" },
		{ "a7", "b_pawn" },
		{ "a8", "b_rook" },
		
		{ "b1", "w_knight" },
		{ "b2", "w_pawn" },
		{ "b3", "empty" },
		{ "b4", "empty" },
		{ "b5", "empty" },
		{ "b6", "empty" },
		{ "b7", "b_pawn" },
		{ "b8", "b_knight" },

		{ "c1", "w_bishop" },
		{ "c2", "w_pawn" },
		{ "c3", "empty" },
		{ "c4", "empty" },
		{ "c5", "empty" },
		{ "c6", "empty" },
		{ "c7", "b_pawn" },
		{ "c8", "b_bishop" },

		{ "d1", "w_queen" },
		{ "d2", "w_pawn" },
		{ "d3", "empty" },
		{ "d4", "empty" },
		{ "d5", "empty" },
		{ "d6", "empty" },
		{ "d7", "b_pawn" },
		{ "d8", "b_queen" },
				
		{ "e1", "w_king" },
		{ "e2", "w_pawn" },
		{ "e3", "empty" },
		{ "e4", "empty" },
		{ "e5", "empty" },
		{ "e6", "empty" },
		{ "e7", "b_pawn" },
		{ "e8", "b_king" },

		{ "f1", "w_bishop" },
		{ "f2", "w_pawn" },
		{ "f3", "empty" },
		{ "f4", "empty" },
		{ "f5", "empty" },
		{ "f6", "empty" },
		{ "f7", "b_pawn" },
		{ "f8", "b_bishop" },

		{ "g1", "w_knight" },
		{ "g2", "w_pawn" },
		{ "g3", "empty" },
		{ "g4", "empty" },
		{ "g5", "empty" },
		{ "g6", "empty" },
		{ "g7", "b_pawn" },
		{ "g8", "b_knight" },

		{ "h1", "w_rook" },
		{ "h2", "w_pawn" },
		{ "h3", "empty" },
		{ "h4", "empty" },
		{ "h5", "empty" },
		{ "h6", "empty" },
		{ "h7", "b_pawn" },
		{ "h8", "b_rook" },
	};
}

GameBoard::~GameBoard()
{

}

void GameBoard::Start()
{
	// Create the Peices
	for (std::shared_ptr<FlatEngine::GameObject> entity : this->GetEntities())
	{
		// Create all board squares and associate them with a square transform in engine
		std::vector<long> childrenIDs = entity->GetChildren();

		// Positions GameObject
		for (long childID : childrenIDs)
		{
			std::shared_ptr<FlatEngine::GameObject> child = FlatEngine::GetObjectById(childID);

			if (child->GetName() == "Positions")
			{
				std::vector<long> grandChildrenIDs = child->GetChildren();

				// a, b, c, d, e, f, g, h
				for (long gChildID : grandChildrenIDs)
				{
					std::shared_ptr<FlatEngine::GameObject> grandChild = FlatEngine::GetObjectById(gChildID);
					std::vector<long> greatChildrenIDs = grandChild->GetChildren();

					// a1, a2, a3, a4, a5, a6, a7, a8, b1, etc...
					for (long greatIDs : greatChildrenIDs)
					{
						std::shared_ptr<FlatEngine::GameObject> greatChild = FlatEngine::GetObjectById(greatIDs);						
						std::shared_ptr<BoardSquare> square = std::make_shared<BoardSquare>();
						square->SetBoardLocation(greatChild);
						
						for (std::map<std::string, std::string>::iterator initialPos = this->initialPositions.begin(); initialPos != this->initialPositions.end(); initialPos++)
						{
							if (greatChild->GetName() == initialPos->first)
							{
								// If space should not be empty
								if (initialPos->second != "empty")
								{
									// Create a new piece
									if (initialPos->second == "w_king")
									{
										std::shared_ptr<FlatEngine::GameObject> piece = FlatEngine::CreateGameObject(greatChild->GetID());

										FlatEngine::LogString("White King Found.");
										std::shared_ptr<King> whiteKing = std::make_shared<King>(square->GetBoardLocation()->GetID());
										whiteKing->SetName("White King");
										whiteKing->SetSprite("assets/images/pieces/white_king.png");
										
										square->SetPiece(piece);
									}
								}
							}
						}
					}
				}
			}
		}
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


