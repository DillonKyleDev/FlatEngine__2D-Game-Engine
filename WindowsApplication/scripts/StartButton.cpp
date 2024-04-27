#include "StartButton.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Sprite.h"
#include "../Audio.h"

#include "../MappingContext.h"
#include "../Transform.h"
#include "../Vector2.h"

StartButton::StartButton()
{
	SetName("StartButton");
}

StartButton::~StartButton()
{
}

void StartButtonOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void StartButtonOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButtonHovered.png");
	//std::shared_ptr<FlatEngine::Audio> clickedAudio = thisObject->GetAudioComponent();
	//clickedAudio->Play();
}

void StartButtonOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//thisObject->GetSpriteComponent()->SetTexture("assets/images/SingleButton.png");
}

void StartButtonOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	//std::shared_ptr<FlatEngine::Audio> clickedAudio = thisObject->GetAudioComponent();
	//clickedAudio->Play();
	//FlatEngine::LoadScene("scenes/ClearGameBoard.json");
}

void StartButtonOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void StartButton::Start()
{
	std::shared_ptr<FlatEngine::Audio> clickedAudio = GetOwner()->GetAudioComponent();
	//clickedAudio->LoadEffect("assets/audio/StartHovered.wav");
	//clickedAudio->SetIsMusic(false);
	
	//std::shared_ptr<FlatEngine::Button> button = GetOwner()->GetButtonComponent();
	// Register Mouse Events functions to the Button
	//button->SetOnMouseOver(StartButtonOnMouseOver);
	//button->SetOnMouseEnter(StartButtonOnMouseEnter);
	//button->SetOnMouseLeave(StartButtonOnMouseLeave);
	//button->SetOnMouseLeftClick(StartButtonOnLeftClick);
	//button->SetOnMouseRightClick(StartButtonOnRightClick);
}

void StartButton::Update(float deltaTime)
{
	// Get Mapping Context
	std::shared_ptr<FlatEngine::MappingContext> mappingContext = FlatEngine::GetMappingContext("TestContext1");
	std::shared_ptr<FlatEngine::Transform> transform = GetOwner()->GetTransformComponent();

	if (mappingContext->GetInputAction("IA_Left").type != 0)
		transform->SetPosition(Vector2(transform->GetPosition().x - 0.05f, transform->GetPosition().y));
	if (mappingContext->GetInputAction("IA_Right").type != 0)
		transform->SetPosition(Vector2(transform->GetPosition().x + 0.05f, transform->GetPosition().y));
	if (mappingContext->GetInputAction("IA_Down").type != 0)
		transform->SetPosition(Vector2(transform->GetPosition().x, transform->GetPosition().y - 0.05f));
	if (mappingContext->GetInputAction("IA_Up").type != 0)
		transform->SetPosition(Vector2(transform->GetPosition().x, transform->GetPosition().y + 0.05f));

	if (mappingContext->Fired("IA_Left"))
		FlatEngine::LogString("Left Fired!");
	if (mappingContext->Fired("IA_Right"))
		FlatEngine::LogString("Right Fired!");
	if (mappingContext->Fired("IA_Down"))
		FlatEngine::LogString("Down Fired!");
	if (mappingContext->Fired("IA_Up"))
		FlatEngine::LogString("Up Fired!");
}