#include "MovingPlatform.h"
#include "FlatEngine.h"


MovingPlatform::MovingPlatform()
{
	SetName("MovingPlatform");
}

MovingPlatform::~MovingPlatform()
{
}

void MovingPlatform::Start()
{
	FlatEngine::LogString("MovingPlatform instantiated on: " + GetOwner()->GetName());
}

void MovingPlatform::Update(float deltaTime)
{

}