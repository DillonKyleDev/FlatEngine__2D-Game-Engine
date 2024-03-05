#pragma once
#include "Piece.h"



class King : public Piece
{
public:
	King(long parentID = -1);
	~King();

	void Start();
	void Update(float deltaTime);

private:

};

