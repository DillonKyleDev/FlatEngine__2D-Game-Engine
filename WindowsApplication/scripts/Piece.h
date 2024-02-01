#pragma once
#include "../GameScript.h"



class Piece : public FlatEngine::GameScript
{
public:
	Piece();
	~Piece();

	void Start();
	void Update(float deltaTime);

private:
	std::string name;
	int level;
};

