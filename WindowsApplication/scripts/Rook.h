#pragma once
#include "Piece.h"



class Rook : public Piece
{
public:
	Rook(long parentID);
	~Rook();

	void Start();
	void Update(float deltaTime);

private:

};