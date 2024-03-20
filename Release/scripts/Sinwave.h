#pragma once
#include "../GameScript.h"



class Sinwave : public FlatEngine::GameScript
{
public:
	Sinwave();
	~Sinwave();

	void Start();
	void Update(float deltaTime);
	
	float amplitudeY = 0;
	float amplitudeX = 100;
};

