#pragma once
#include "Piece.h"



class Queen : public Piece
{
public:
	Queen(long parentID = -1);
	~Queen();

	void Start();
	void Update(float deltaTime);

private:

};