#pragma once
#include "../GameScript.h"

class PlayerController : public FlatEngine::GameScript
{
public:
	PlayerController();
	~PlayerController();

	void Start();
	void Update(float deltaTime);

private:
};