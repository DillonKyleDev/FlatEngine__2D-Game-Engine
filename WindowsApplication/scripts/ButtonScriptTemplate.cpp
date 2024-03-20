#include "ButtonScriptTemplate.h"
#include "../FlatEngine.h"


ButtonScriptTemplate::ButtonScriptTemplate()
{
	this->SetName("ButtonScriptTemplate");
}

ButtonScriptTemplate::~ButtonScriptTemplate()
{
}

void ButtonScriptTemplateOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Over... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void ButtonScriptTemplateOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Enter... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void ButtonScriptTemplateOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Leave... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void ButtonScriptTemplateOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Left Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void ButtonScriptTemplateOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Right Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void ButtonScriptTemplate::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		// Register Mouse Events functions to the Button
		button->SetOnMouseOver(ButtonScriptTemplateOnMouseOver);
		button->SetOnMouseEnter(ButtonScriptTemplateOnMouseEnter);
		button->SetOnMouseLeave(ButtonScriptTemplateOnMouseLeave);
		button->SetOnMouseLeftClick(ButtonScriptTemplateOnLeftClick);
		button->SetOnMouseRightClick(ButtonScriptTemplateOnRightClick);
	}
}


void ButtonScriptTemplate::Update(float deltaTime)
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

