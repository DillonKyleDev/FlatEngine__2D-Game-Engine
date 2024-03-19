#pragma once
#include "../GameScript.h"

class AI : public FlatEngine::GameScript
{
public:
	AI();
	~AI();

	void Start();
	void Update(float deltaTime);

private:
	std::string turnColor;
};

