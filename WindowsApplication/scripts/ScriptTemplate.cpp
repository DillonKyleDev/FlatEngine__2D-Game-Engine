#include "ScriptTemplate.h"
#include "../FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.cpp and 
// Add a new if branch for the script name in GameLoop()::Start() function


ScriptTemplate::ScriptTemplate()
{
	this->SetName("ScriptTemplate");
}

ScriptTemplate::~ScriptTemplate()
{

}

void ScriptTemplate::Start()
{
	FlatEngine::LogString("ScriptTemplate instantiated on: " + this->GetOwner()->GetName());
}

void ScriptTemplate::Update(int deltaTime)
{

}