#pragma once
#include "../GameScript.h"

class QuitButton : public FlatEngine::GameScript
{
public:
	QuitButton();
	~QuitButton();

	void Start();
	void Update(int deltaTime);

private:

};