#pragma once
#include "../GameScript.h"


class Attack : public FlatEngine::GameScript
{
public:
	Attack();
	~Attack();

	void Start();
	void Update(float deltaTime);
};

