#include "King.h"

King::King(long parentID)
{
	this->SetName("King");
	this->CreatePieceObject(parentID);
}

King::~King()
{
}

void King::Start()
{
}

void King::Update(float deltaTime)
{
}
