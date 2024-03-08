#include "Pawn.h"

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
