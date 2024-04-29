#include "StartButton.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Sprite.h"
#include "../Audio.h"
#include "../CharacterController.h"

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
	std::shared_ptr<FlatEngine::MappingContext> mappingContext = FlatEngine::GetMappingContext("MC_CharacterContext.json");
	std::shared_ptr<FlatEngine::CharacterController> characterController = GetOwner()->GetCharacterController();

	if (mappingContext->Fired("IA_Jump"))
		FlatEngine::LogString("Jumped!");

	SDL_Event moveX = mappingContext->GetInputAction("IA_MoveX");
	SDL_Event moveY = mappingContext->GetInputAction("IA_MoveY");
	static int xDir = 0;
	static int yDir = 0;

	if (moveX.type != 0)
		xDir = moveX.jaxis.value;
	if (moveY.type != 0)
		yDir = moveY.jaxis.value;

	if (characterController != nullptr)
		characterController->MoveToward(Vector2(xDir, yDir));
}