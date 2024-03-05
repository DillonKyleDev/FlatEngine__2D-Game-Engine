#pragma once
#include "Piece.h"



class Rook : public Piece
{
public:
	Rook(long parentID = -1);
	~Rook();

	void Start();
	void Update(float deltaTime);

private:

};