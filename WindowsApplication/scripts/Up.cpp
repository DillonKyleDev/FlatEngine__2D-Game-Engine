#include "Up.h"
#include "../FlatEngine.h"


Up::Up()
{

}

Up::~Up()
{

}

void UpOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Over... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));

	Vector2 position = transform->GetPosition();
	transform->SetScale(Vector2(2,2));
	//transform->SetPosition(Vector2(position.x + 1, position.y));
}

void UpOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Left Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));

	//for (int i = 0; i < this->GetEntities().size())
}

void UpOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Right Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void Up::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		// Register Mouse Events functions to the Button
		button->OnMouseOver(UpOnMouseOver);
		button->OnMouseLeftClick(UpOnLeftClick);
		button->OnMouseRightClick(UpOnRightClick);
	}
}


void Up::Update(float deltaTime)
{
	// For all entities attatched to this script:
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
	}
}

