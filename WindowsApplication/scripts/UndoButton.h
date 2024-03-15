#pragma once
#include "../GameScript.h"

class UndoButton : public FlatEngine::GameScript
{
public:
	UndoButton();
	~UndoButton();

	void Start();
	void Update(float deltaTime);

private:

};