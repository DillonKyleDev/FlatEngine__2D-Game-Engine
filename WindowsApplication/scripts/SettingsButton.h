#pragma once
#include "../FlatEngine.h"
#include "../GameScript.h"


class SettingsButton : public FlatEngine::GameScript
{
public:
	SettingsButton(long ownerID);
	~SettingsButton();

	void Start();
	void Update(float deltaTime);
};

