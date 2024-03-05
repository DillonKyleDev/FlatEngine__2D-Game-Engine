#pragma once
#include "Piece.h"



class Queen : public Piece
{
public:
	Queen(long parentID);
	~Queen();

	void Start();
	void Update(float deltaTime);

private:

};