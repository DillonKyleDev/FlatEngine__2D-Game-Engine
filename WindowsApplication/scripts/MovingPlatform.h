#pragma once
#include "GameScript.h"


class MovingPlatform : public FlatEngine::GameScript
{
public:
	MovingPlatform();
	~MovingPlatform();

	void Start();
	void Update(float deltaTime);
};