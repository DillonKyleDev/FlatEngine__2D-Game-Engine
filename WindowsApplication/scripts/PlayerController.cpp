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
	mappingContext = nullptr;
	characterController = nullptr;
	rigidBody = nullptr;
	transform = nullptr;
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	mappingContext = FlatEngine::GetMappingContext("MC_CharacterContext.json");
	characterController = GetOwner()->GetCharacterController();
	rigidBody = GetOwner()->GetRigidBody();
	transform = GetOwner()->GetTransformComponent();
}

void PlayerController::Update(float deltaTime)
{
	bool _moving = false;
	static int xDir = 0;
	static int yDir = 0;
	Vector2 velocity = Vector2(0, 0);
	if (rigidBody != nullptr)
		velocity = rigidBody->GetVelocity();

	if (mappingContext != nullptr && rigidBody != nullptr && mappingContext->Fired("IA_Jump"))
	{
		if (rigidBody->IsGrounded() && velocity.y < 0.001f && velocity.y > -0.001f)
		{
			rigidBody->AddForce(Vector2(0, 1), 1300);
		}
	}
	if (mappingContext != nullptr && characterController != nullptr && mappingContext->GetInputAction("IA_MoveLeft").type != 0)
	{
		xDir = -30000;
		characterController->MoveToward(Vector2(-36000, 0));
		_moving = true;
	}
	if (mappingContext != nullptr && characterController != nullptr && mappingContext->GetInputAction("IA_MoveRight").type != 0)
	{
		xDir = 30000;
		characterController->MoveToward(Vector2(36000, 0));
		_moving = true;
	}
	//if (mappingContext->GetInputAction("IA_MoveUp").type != 0)
	//	characterController->MoveToward(Vector2(0, 1));
	//if (mappingContext->GetInputAction("IA_MoveDown").type != 0)
	//	characterController->MoveToward(Vector2(0, -1));

	//xDir = 30000;
	//characterController->MoveToward(Vector2(36000, 0));
	//_moving = true;

	SDL_Event moveX = SDL_Event();
	SDL_Event moveY = SDL_Event();

	if (mappingContext != nullptr)
	{
		{
			moveX = mappingContext->GetInputAction("IA_MoveX");
			moveY = mappingContext->GetInputAction("IA_MoveY");
		} 
	}

	if (moveX.type != 0)
		xDir = moveX.jaxis.value;
	if (moveY.type != 0)
		yDir = moveY.jaxis.value;

	if (characterController != nullptr)
	{
		//characterController->MoveToward(Vector2((float)xDir, (float)yDir));
	}
	//FlatEngine::RayCast(transform->GetPosition(), Vector2((float)xDir, -(float)yDir), 2);

	characterController->SetMoving(_moving);
}