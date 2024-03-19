#include "RestartButton.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Audio.h"

RestartButton::RestartButton()
{

}

RestartButton::~RestartButton()
{
}

void RestartButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void RestartButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButtonHovered.png");
	std::shared_ptr<FlatEngine::Audio> clickedAudio = std::static_pointer_cast<FlatEngine::Audio>(thisObject->GetComponent(FlatEngine::ComponentTypes::Audio));
	clickedAudio->Play();
}

void RestartButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButton.png");
}

void RestartButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	std::shared_ptr<FlatEngine::Audio> clickedAudio = std::static_pointer_cast<FlatEngine::Audio>(thisObject->GetComponent(FlatEngine::ComponentTypes::Audio));
	clickedAudio->Play();
	FlatEngine::LoadScene("ClearGameBoard.json");
}

void RestartButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}


void RestartButton::Start()
{
	std::shared_ptr<FlatEngine::Audio> clickedAudio = std::static_pointer_cast<FlatEngine::Audio>(GetOwner()->AddComponent(FlatEngine::ComponentTypes::Audio));
	clickedAudio->LoadEffect("assets/audio/StartHovered.wav");
	clickedAudio->SetIsMusic(false);

	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(this->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(RestartButtonOnMouseOver);
	button->SetOnMouseEnter(RestartButtonOnMouseEnter);
	button->SetOnMouseLeave(RestartButtonOnMouseLeave);
	button->SetOnMouseLeftClick(RestartButtonOnLeftClick);
	button->SetOnMouseRightClick(RestartButtonOnRightClick);
}

void RestartButton::Update(float deltaTime)
{
}