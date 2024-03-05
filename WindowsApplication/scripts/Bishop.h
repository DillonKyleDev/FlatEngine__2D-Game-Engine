#pragma once
#include "Piece.h"



class Bishop : public Piece
{
public:
	Bishop(long parentID);
	~Bishop();

	void Start();
	void Update(float deltaTime);

private:

};