#pragma once
#include "GameScript.h"

class StartButton : public FlatEngine::GameScript
{
public:
	StartButton(long ownerID);
	~StartButton();

	void Start();
	void Update(float deltaTime);

private:
};