#pragma once
#include "../GameScript.h"


class Up : public FlatEngine::GameScript
{
public:
	Up();
	~Up();

	void Start();
	void Update(float deltaTime);
};

