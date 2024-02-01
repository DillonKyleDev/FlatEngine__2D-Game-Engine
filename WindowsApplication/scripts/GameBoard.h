#pragma once
#include "../GameScript.h"
#include <map>
#include <string>


class GameBoard : public FlatEngine::GameScript
{
public:
	GameBoard();
	~GameBoard();

	void Start();
	void Update(float deltaTime);

private:
	std::map<std::string, int> squares;
};

