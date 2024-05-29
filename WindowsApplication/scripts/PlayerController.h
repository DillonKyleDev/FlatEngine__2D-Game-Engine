#pragma once
#include "../GameScript.h"

class PlayerController : public FlatEngine::GameScript
{
public:
	PlayerController();
	~PlayerController();

	void Start();
	void Update(int deltaTime);

private:
};