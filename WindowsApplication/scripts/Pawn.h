#pragma once
#include "Piece.h"



class Pawn : public Piece
{
public:
	Pawn(long parentID = -1);
	~Pawn();

	void Start();
	void Update(float deltaTime);

private:

};

