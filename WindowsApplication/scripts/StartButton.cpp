#include "StartButton.h"
#include "../FlatEngine.h"


StartButton::StartButton()
{
	// Play Background music
	this->backgroundAudio = std::make_shared<FlatEngine::Sound>();
	this->backgroundAudio->LoadMusic("assets/audio/WindLoop1.wav");
	this->backgroundAudio->PlayMusic();
}

StartButton::~StartButton()
{
}

void StartButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{	
}

void StartButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	// Play Sound Effect
	std::shared_ptr<FlatEngine::Sound> startHovered = std::make_shared<FlatEngine::Sound>();
	startHovered->LoadEffect("assets/audio/StartHovered.wav");
	startHovered->PlayEffect();

	// Increase scale of button
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
	static Vector2 originalScale = transform->GetScale();
	transform->SetScale(Vector2(originalScale.x * 1.05f, originalScale.y * 1.05f));
}

void StartButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	// Decrease scale of button
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
	static Vector2 originalScale = transform->GetScale();
	transform->SetScale(Vector2(originalScale.x / 1.05f, originalScale.y / 1.05f));
}

void StartButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	// Play Sound Effect
	std::shared_ptr<FlatEngine::Sound> startClicked = std::make_shared<FlatEngine::Sound>();
	startClicked->LoadEffect("assets/audio/StartClicked.wav");
	startClicked->PlayEffect();

	// Stop background music
	if (startClicked->IsMusicPlaying())
		startClicked->StopMusic();

	// Load into fightingUI scene
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
	FlatEngine::LoadScene("fightingUI.json");
}

void StartButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void StartButton::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		// Register Mouse Events functions to the Button
		button->OnMouseOver(StartButtonOnMouseOver);
		button->OnMouseEnter(StartButtonOnMouseEnter);
		button->OnMouseLeave(StartButtonOnMouseLeave);
		button->OnMouseLeftClick(StartButtonOnLeftClick);
		button->OnMouseRightClick(StartButtonOnRightClick);
	}
}


void StartButton::Update(float deltaTime)
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

