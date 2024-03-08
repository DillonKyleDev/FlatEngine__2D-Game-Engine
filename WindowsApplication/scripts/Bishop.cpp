#include "Bishop.h"

Bishop::Bishop(long parentID)
{
	this->CreatePieceObject(parentID);
	this->SetName("Bishop");
}

Bishop::~Bishop()
{
}

void Bishop::Start()
{
}

void Bishop::Update(float deltaTime)
{
}
