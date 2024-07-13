#pragma once
#include "../FlatEngine.h"
#include "../GameScript.h"


class ButtonScriptTemplate : public FlatEngine::GameScript
{
public:
	ButtonScriptTemplate(long ownerID);
	~ButtonScriptTemplate();

	void Start();
	void Update(float deltaTime);
};

