#include "Up.h"
#include "../FlatEngine.h"
#include "../Animation.h"


Up::Up()
{

}

Up::~Up()
{
}

void UpOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//FlatEngine::LogString("Mouse Over... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void UpOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//FlatEngine::LogString("Mouse Leave... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void UpOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//FlatEngine::LogString("Left Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
	Vector2 position = transform->GetPosition();

	// Get Animation Component
	std::shared_ptr<FlatEngine::Animation> animation = std::static_pointer_cast<FlatEngine::Animation>(thisObject->GetComponent(FlatEngine::ComponentTypes::Animation));

	if (animation != nullptr)
		animation->Play();
}

void UpOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//FlatEngine::LogString("Right Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void Up::Start()
{
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(this->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));

	if (button != nullptr)
	{
		// Register Mouse Events functions to the Button
		button->OnMouseOver(UpOnMouseOver);
		button->OnMouseLeave(UpOnMouseLeave);
		button->OnMouseLeftClick(UpOnLeftClick);
		button->OnMouseRightClick(UpOnRightClick);
	}
}


void Up::Update(float deltaTime)
{
	std::shared_ptr<FlatEngine::GameObject> owner = this->GetOwner();
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(owner->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
	Vector2 position = transform->GetPosition();
	float xPos = position.x;
	float yPos = position.y;

	transform->SetPosition(Vector2(xPos, yPos + .1));
}

