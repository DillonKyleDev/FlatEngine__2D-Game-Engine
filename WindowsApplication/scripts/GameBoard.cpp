#include "GameBoard.h"
#include "../FlatEngine.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// New the variable in GameLoop()::Start() function and delete it in GameLoop()::Stop();
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


GameBoard::GameBoard()
{

}

GameBoard::~GameBoard()
{

}

void GameBoard::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		FlatEngine::LogString("GameBoard instantiated on: " + this->GetEntities()[i]->GetName());
	}
}

void GameBoard::Update(float deltaTime)
{
	std::vector<std::shared_ptr<FlatEngine::GameObject>> attatchedEntities = this->GetEntities();
	for (int i = 0; i < attatchedEntities.size(); i++)
	{
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(attatchedEntities[i]->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
		transform->SetPosition(Vector2(xPos, yPos + 1));
	}
}