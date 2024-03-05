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

void BoardSquare::SetPiece(std::shared_ptr<FlatEngine::GameObject> piece)
{
	this->piece = piece;
	this->boardLocation->AddChild(this->piece->GetID());
}

std::shared_ptr<FlatEngine::GameObject> BoardSquare::GetPiece()
{
	return this->piece;
}
