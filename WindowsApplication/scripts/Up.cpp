#include "Up.h"
#include "../FlatEngine.h"
#include "../Animation.h"


Up::Up()
{
	this->SetName("Up");
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
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		if (button != nullptr)
		{
			// Register Mouse Events functions to the Button
			button->SetOnMouseOver(UpOnMouseOver);
			button->SetOnMouseLeave(UpOnMouseLeave);
			button->SetOnMouseLeftClick(UpOnLeftClick);
			button->SetOnMouseRightClick(UpOnRightClick);
		}
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

