#include "PlayerController.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Sprite.h"
#include "../Audio.h"
#include "../CharacterController.h"
#include "../RigidBody.h"
#include "imgui.h"

#include "../MappingContext.h"
#include "../Transform.h"
#include "../Vector2.h"

PlayerController::PlayerController()
{
	SetName("PlayerController");
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	std::shared_ptr<FlatEngine::Audio> clickedAudio = GetOwner()->GetAudioComponent();
}

void PlayerController::Update(int deltaTime)
{
	// Get Mapping Context
	std::shared_ptr<FlatEngine::MappingContext> mappingContext = FlatEngine::GetMappingContext("MC_CharacterContext.json");
	std::shared_ptr<FlatEngine::CharacterController> characterController = GetOwner()->GetCharacterController();
	std::shared_ptr<FlatEngine::RigidBody> rigidBody = GetOwner()->GetRigidBody();
	std::shared_ptr<FlatEngine::Transform> transform = GetOwner()->GetTransformComponent();

	bool _moving = false;
	static int xDir = 0;
	static int yDir = 0;
	Vector2 velocity = rigidBody->GetVelocity();

	if (mappingContext->Fired("IA_Jump"))
	{
		//FlatEngine::LogString("Jumped!");
		if (rigidBody->IsGrounded() && velocity.y < 0.001f && velocity.y > -0.001f)
		{
			rigidBody->AddForce(Vector2(0, 1), 0.8f);
		}

	}
	if (mappingContext->GetInputAction("IA_MoveLeft").type != 0)
	{
		xDir = -30000;
		characterController->MoveToward(Vector2(-36000, 0));
		_moving = true;
	}
	if (mappingContext->GetInputAction("IA_MoveRight").type != 0)
	{
		xDir = 30000;
		characterController->MoveToward(Vector2(36000, 0));
		_moving = true;
	}
	//if (mappingContext->GetInputAction("IA_MoveUp").type != 0)
	//	characterController->MoveToward(Vector2(0, 1));
	//if (mappingContext->GetInputAction("IA_MoveDown").type != 0)
	//	characterController->MoveToward(Vector2(0, -1));

	SDL_Event moveX = mappingContext->GetInputAction("IA_MoveX");
	SDL_Event moveY = mappingContext->GetInputAction("IA_MoveY");


	if (moveX.type != 0)
		xDir = moveX.jaxis.value;
	if (moveY.type != 0)
		yDir = moveY.jaxis.value;

	if (characterController != nullptr)
	{
		//characterController->MoveToward(Vector2((float)xDir, (float)yDir));
	}
	FlatEngine::RayCast(transform->GetPosition(), Vector2((float)xDir, -(float)yDir), 2);

	characterController->SetMoving(_moving);
}