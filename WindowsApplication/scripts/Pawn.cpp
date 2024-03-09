#include "Pawn.h"
#include "../FlatEngine.h"

Pawn::Pawn(long parentID)
{
	this->SetName("Pawn");
	this->CreatePieceObject(parentID);
}

Pawn::~Pawn()
{
}

void Pawn::Start()
{
}

void Pawn::Update(float deltaTime)
{
}

std::vector<std::shared_ptr<BoardSquare>> Pawn::GetPossibleMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares)
{

}
