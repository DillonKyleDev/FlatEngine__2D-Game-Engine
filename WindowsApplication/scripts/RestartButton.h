#pragma once
#include "../GameScript.h"

class RestartButton : public FlatEngine::GameScript
{
public:
	RestartButton();
	~RestartButton();

	void Start();
	void Update(int deltaTime);

private:

};