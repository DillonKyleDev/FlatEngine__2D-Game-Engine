#include "ButtonScriptTemplate.h"
#include "FlatEngine.h"
#include "Transform.h"


ButtonScriptTemplate::ButtonScriptTemplate(long ownerID) : GameScript(ownerID)
{
	SetName("ButtonScriptTemplate");
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
	FlatEngine::LogString("ButtonScriptTemplate instantiated on: " + GetOwner()->GetName());
	std::shared_ptr<FlatEngine::Button> button = GetOwner()->GetButtonComponent();

	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(ButtonScriptTemplateOnMouseOver);
	button->SetOnMouseEnter(ButtonScriptTemplateOnMouseEnter);
	button->SetOnMouseLeave(ButtonScriptTemplateOnMouseLeave);
	button->SetOnMouseLeftClick(ButtonScriptTemplateOnLeftClick);
	button->SetOnMouseRightClick(ButtonScriptTemplateOnRightClick);
}


void ButtonScriptTemplate::Update(float deltaTime)
{
	//std::shared_ptr<FlatEngine::Transform> transform = GetOwner()->GetTransformComponent();
	//Vector2 position = transform->GetPosition();
	//float xPos = position.x;
	//float yPos = position.y;
}

