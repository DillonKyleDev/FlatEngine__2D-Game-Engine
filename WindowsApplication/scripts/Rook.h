#pragma once
#include "Piece.h"
#include "BoardSquare.h"


class Rook : public Piece
{
public:
	Rook(long parentID = -1);
	~Rook();

	void Start();
	void Update(float deltaTime);
	std::vector<std::shared_ptr<BoardSquare>> GetPossibleMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares);

private:

};