#include "Rook.h"
#include "../FlatEngine.h"

Rook::Rook(long parentID)
{
	this->SetName("Rook");
	this->CreatePieceObject(parentID);
}

Rook::~Rook()
{
}

void Rook::Start()
{
}

void Rook::Update(float deltaTime)
{
}

std::vector<std::shared_ptr<BoardSquare>> Rook::GetPossibleMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;

	int colCounter = 0;
	int rowCounter = 0;

	for (int i = 0; i < 64; i++)
	{

	}

	return availableSquares;
}