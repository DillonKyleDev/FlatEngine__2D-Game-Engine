#include "SettingsButton.h"
#include "../FlatEngine.h"
#include "../Transform.h"


SettingsButton::SettingsButton()
{
	SetName("SettingsButton");
}

SettingsButton::~SettingsButton()
{
}

void SettingsButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Over... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void SettingsButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Enter... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void SettingsButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Leave... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void SettingsButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Left Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void SettingsButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Right Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void SettingsButton::Start()
{	
	FlatEngine::LogString("SettingsButton instantiated on: " + GetOwner()->GetName());
	std::shared_ptr<FlatEngine::Button> button = GetOwner()->GetButtonComponent();	

	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(SettingsButtonOnMouseOver);
	button->SetOnMouseEnter(SettingsButtonOnMouseEnter);
	button->SetOnMouseLeave(SettingsButtonOnMouseLeave);
	button->SetOnMouseLeftClick(SettingsButtonOnLeftClick);
	button->SetOnMouseRightClick(SettingsButtonOnRightClick);
}


void SettingsButton::Update(float deltaTime)
{
	//std::shared_ptr<FlatEngine::Transform> transform = GetOwner()->GetTransformComponent();
	//Vector2 position = transform->GetPosition();
	//float xPos = position.x;
	//float yPos = position.y;
}

