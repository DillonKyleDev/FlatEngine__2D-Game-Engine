#pragma once
#include "../GameScript.h"
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
	std::map<std::string, BoardSquare> squares;
	std::map<std::string, std::string> initialPositions;
};

