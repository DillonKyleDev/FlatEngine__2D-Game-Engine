#pragma once
#include "FlatEngine.h"
#include "GameScript.h"

class RestartButton : public FlatEngine::GameScript
{
public:
	RestartButton(long ownerID);
	~RestartButton();

	void Start();
	void Update(float deltaTime);

private:

};