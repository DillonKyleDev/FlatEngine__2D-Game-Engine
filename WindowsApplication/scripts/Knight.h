#pragma once
#include "Piece.h"



class Knight : public Piece
{
public:
	Knight(long parentID = -1);
	~Knight();

	void Start();
	void Update(float deltaTime);

private:

};