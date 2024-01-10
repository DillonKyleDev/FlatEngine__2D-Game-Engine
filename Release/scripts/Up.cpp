#include "Up.h"
#include "../FlatEngine.h"


Up::Up()
{

}

Up::~Up()
{

}

void Up::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		FlatEngine::LogString("Up instantiated on: " + this->GetEntities()[i]->GetName());
	}
}

void Up::Update(float deltaTime)
{
	std::vector<FlatEngine::GameObject*> attatchedEntities = this->GetEntities();
	for (int i = 0; i < attatchedEntities.size(); i++)
	{
		FlatEngine::Transform* transform = static_cast<FlatEngine::Transform*>(attatchedEntities[i]->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
		FlatEngine::LogFloat(deltaTime, "deltaTime: ");
		transform->SetPosition(Vector2(xPos, yPos + .2f * deltaTime));
	}
}