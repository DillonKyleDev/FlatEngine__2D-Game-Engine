#pragma once
#include "../GameScript.h"


class SettingsButton : public FlatEngine::GameScript
{
public:
	SettingsButton();
	~SettingsButton();

	void Start();
	void Update(float deltaTime);
};

