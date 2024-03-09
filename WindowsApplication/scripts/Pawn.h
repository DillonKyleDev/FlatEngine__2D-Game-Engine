#pragma once
#include "Piece.h"
#include "BoardSquare.h"


class Pawn : public Piece
{
public:
	Pawn(long parentID = -1);
	~Pawn();

	void Start();
	void Update(float deltaTime);
	std::vector<std::shared_ptr<BoardSquare>> GetPossibleMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares);

private:

};

