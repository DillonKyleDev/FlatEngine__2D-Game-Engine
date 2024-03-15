#include "UndoButton.h"
#include "../Button.h"
#include "../FlatEngine.h"

UndoButton::UndoButton()
{
}

UndoButton::~UndoButton()
{
}

void UndoButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{

}

void UndoButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void UndoButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void UndoButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::gameManager->UndoMove();
	FlatEngine::gameManager->ChangeTurns();
}

void UndoButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}


void UndoButton::Start()
{
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(this->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));

	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(UndoButtonOnMouseOver);
	button->SetOnMouseEnter(UndoButtonOnMouseEnter);
	button->SetOnMouseLeave(UndoButtonOnMouseLeave);
	button->SetOnMouseLeftClick(UndoButtonOnLeftClick);
	button->SetOnMouseRightClick(UndoButtonOnRightClick);
}

void UndoButton::Update(float deltaTime)
{
}