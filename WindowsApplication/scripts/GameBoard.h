#pragma once
#include "../GameScript.h"
#include "../GameObject.h"
#include <map>
#include <string>
#include "BoardSquare.h"


class GameBoard : public FlatEngine::GameScript
{
public:
	GameBoard();
	~GameBoard();

	void Start();
	void Update(float deltaTime);

private:
	std::vector<long> squareIDs;
	std::vector<std::vector<std::shared_ptr<FlatEngine::GameObject>>> boardSquares;
};

