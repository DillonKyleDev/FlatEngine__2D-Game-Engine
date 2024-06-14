#pragma once
#include "GameScript.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.cpp and 
// Add a new if branch for the script name in GameLoop()::Start() function


class ScriptTemplate : public FlatEngine::GameScript
{
public:
	ScriptTemplate();
	~ScriptTemplate();

	void Start();
	void Update(float deltaTime);
};

