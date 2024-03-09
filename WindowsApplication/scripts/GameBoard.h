#pragma once
#include "../GameScript.h"
#include "BoardSquare.h"
#include <map>
#include <string>


class GameBoard : public FlatEngine::GameScript
{
public:
	GameBoard();
	~GameBoard();

	void Awake();
	void Start();
	void Update(float deltaTime);

	void TakePiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> movingFrom);
	void MovePiece(std::shared_ptr<BoardSquare> movingTo, std::shared_ptr<BoardSquare> movingFrom);
	void SetupBoard();
	std::vector<std::shared_ptr<BoardSquare>> GetBoardSquares();
	std::vector<std::shared_ptr<BoardSquare>> GetWhiteSquares();
	std::vector<std::shared_ptr<BoardSquare>> GetBlackSquares();

private:
	std::vector<long> squareIDs;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares;
	std::vector<std::shared_ptr<BoardSquare>> whiteSquares;
	std::vector<std::shared_ptr<BoardSquare>> blackSquares;
};

