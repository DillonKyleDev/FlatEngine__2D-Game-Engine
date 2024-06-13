#pragma once
#include "GameScript.h"


class ButtonScriptTemplate : public FlatEngine::GameScript
{
public:
	ButtonScriptTemplate();
	~ButtonScriptTemplate();

	void Start();
	void Update(float deltaTime);
};

