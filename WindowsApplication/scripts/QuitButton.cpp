#include "QuitButton.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Audio.h"
#include "../Sprite.h"

QuitButton::QuitButton()
{
	this->SetName("QuitButton");
}

QuitButton::~QuitButton()
{
}

void QuitButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("MouseOver!");
}

void QuitButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButtonHovered.png");
	//std::shared_ptr<FlatEngine::Audio> clickedAudio = std::static_pointer_cast<FlatEngine::Audio>(thisObject->GetComponent(FlatEngine::ComponentTypes::Audio));
	//clickedAudio->Play();
}

void QuitButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButton.png");
}

void QuitButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::CloseProgram();
}

void QuitButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}


void QuitButton::Start()
{
	//std::shared_ptr<FlatEngine::Audio> clickedAudio = std::static_pointer_cast<FlatEngine::Audio>(GetOwner()->AddComponent(FlatEngine::ComponentTypes::Audio));
	//clickedAudio->LoadEffect("assets/audio/StartHovered.wav");
	//clickedAudio->SetIsMusic(false);

	std::shared_ptr<FlatEngine::Button> button = GetOwner()->GetButtonComponent();
	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(QuitButtonOnMouseOver);
	button->SetOnMouseEnter(QuitButtonOnMouseEnter);
	button->SetOnMouseLeave(QuitButtonOnMouseLeave);
	button->SetOnMouseLeftClick(QuitButtonOnLeftClick);
	button->SetOnMouseRightClick(QuitButtonOnRightClick);
}

void QuitButton::Update(float deltaTime)
{
}