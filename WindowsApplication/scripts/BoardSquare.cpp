#include "BoardSquare.h"

BoardSquare::BoardSquare()
{
}

BoardSquare::~BoardSquare()
{
}

void BoardSquare::Start()
{
}

void BoardSquare::Update(float deltaTime)
{
}

void BoardSquare::SetBoardLocation(std::shared_ptr<FlatEngine::GameObject> boardLocation)
{
	this->boardLocation = boardLocation;
}

std::shared_ptr<FlatEngine::GameObject> BoardSquare::GetBoardLocation()
{
	return this->boardLocation;
}

void BoardSquare::SetPiece(std::shared_ptr<Piece> piece)
{
	this->piece = piece;
	this->piece->PlaceOnSquare(this->boardLocation);
}

std::shared_ptr<Piece> BoardSquare::GetPiece()
{
	return this->piece;
}
