#include "PlayerController.h"
#include "Button.h"
#include "FlatEngine.h"
#include "Sprite.h"
#include "Audio.h"
#include "CharacterController.h"
#include "RigidBody.h"
#include "MappingContext.h"
#include "Vector2.h"
#include "GameObject.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "Animation.h"

#include "Health.h"
#include "BlobParticle.h"

#include "imgui.h"

PlayerController::PlayerController(long ownerID) : GameScript(ownerID)
{
	SetName("PlayerController");
	health = nullptr;
	mappingContext = nullptr;
	characterController = nullptr;
	boxCollider = nullptr;
	rigidBody = nullptr;
	transform = nullptr;
	animator = nullptr;
	sprite = nullptr;
	audio = nullptr;
	whipArm = nullptr;
}

PlayerController::~PlayerController()
{
}

void WhipAttackStart(std::shared_ptr<FlatEngine::GameObject> thisObject)
{	
	thisObject->GetBoxCollider()->SetActive(true);
}

void WhipAttackEnd(std::shared_ptr<FlatEngine::GameObject> thisObject)
{	
	thisObject->GetBoxCollider()->SetActive(false);
}

void OnWhipAttackConnect(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	if (collidedWith->HasComponent("RigidBody") && collidedWith->HasComponent("Transform"))
	{
		Vector2 collidedPosition = collidedWith->GetTransformComponent()->GetTruePosition();
		Vector2 thisPosition = thisObject->GetTransformComponent()->GetTruePosition();
		float awayFromMeX = collidedPosition.x - thisPosition.x;
		Vector2 normalizedAway = Vector2(0, 1);
		if (awayFromMeX > 0)
			normalizedAway.x = 1;
		else
			normalizedAway.x = -1;

		collidedWith->GetRigidBody()->AddForce(normalizedAway, 500);

		thisObject->GetAudioComponent()->LoadEffect("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/audio/Spark13.wav");
		thisObject->GetAudioComponent()->Play();
	}

	if (collidedWith->HasComponent("Script"))
	{
		//std::shared_ptr<Health> enemyHealth = std::static_pointer_cast<Health>(collidedWith->GetGameScriptByName("Health"));
		//if (enemyHealth != nullptr)
		//{
		//	enemyHealth->Damage(thisObject, 20);
		//}
	}
}

void PlayerController::Start()
{
	//health = std::static_pointer_cast<Health>(GetOwner()->GetGameScriptByName("Health"));
	//mappingContext = FlatEngine::GetMappingContext("MC_CharacterContext.json");
	//characterController = GetOwner()->GetCharacterController();
	//boxCollider = GetOwner()->GetBoxCollider();
	//rigidBody = GetOwner()->GetRigidBody();
	//transform = GetOwner()->GetTransformComponent();
	//sprite = GetOwner()->GetSpriteComponent();
	//audio = GetOwner()->GetAudioComponent();
	//audio->SetPath("assets/audio/lazerFire.wav");
	//audio->SetIsMusic(false);

	// Whip arm
	//if (GetOwner()->GetFirstChild()->GetName() == "WhipArm")
	//	whipArm = GetOwner()->GetFirstChild();
	//animator = whipArm->GetAnimationComponent();
	//animator->AddEventFunction("WhipAttackStart", WhipAttackStart);
	//animator->AddEventFunction("WhipAttackEnd", WhipAttackEnd);
	//whipArm->GetBoxCollider()->SetOnCollisionEnter(OnWhipAttackConnect);

	// Swinging
	//spring.attachedTo = FlatEngine::GetObjectByName("Grab");
	//spring.radius = 10.0f;
}

void PlayerController::Update(float deltaTime)
{
	HandleInput();

	//Vector2 attachedPos = spring.attachedTo->GetTransformComponent()->GetTruePosition();
	//Vector2 myPos = GetOwner()->GetTransformComponent()->GetTruePosition();

	//// Calculate center distance with pythag
	//float rise = attachedPos.y - myPos.y;
	//float run = attachedPos.x - myPos.x;
	//float distance = std::sqrt((rise * rise) + (run * run));
	//float damp = 0.1f;
	//float dir = rise * 0.1f * (distance - spring.radius) * damp;

	//
	//if (spring.radius <= distance)
	//{
	//	Vector2 direction = Vector2(run, rise).Normalize();
	//	Vector2 correctedDirection = Vector2(direction.x, direction.y * rigidBody->GetPendingForces().y);
	//	FlatEngine::LogVector2(direction, "Direction: ");
	//	//rigidBody->AddVelocity(Vector2(0, 1));
	//	//FlatEngine::LogVector2(direction);
	//	rigidBody->SetPendingForces(correctedDirection);
	//}
}

void PlayerController::HandleInput()
{
	bool _moving = false;
	bool _isGrounded = false;
	int xDir = 0;
	int yDir = 0;
	Vector2 velocity = Vector2(0, 0);
	FlatEngine::Animation* animator = GetOwner()->GetAnimationComponent();

	if (rigidBody != nullptr)
	{
		velocity = rigidBody->GetVelocity();
		_isGrounded = rigidBody->IsGrounded();
	}

	if (mappingContext != nullptr)
	{
		if (mappingContext->Fired("IA_Jump"))
		{
			//if (_isGrounded && velocity.y < 0.001f && velocity.y > -0.001f)
			//{
				rigidBody->SetPendingForces(Vector2(rigidBody->GetPendingForces().x, 0));
				rigidBody->AddForce(Vector2(0, 1), 1700);
				animator->Play();
			//}
		}
		if (mappingContext->Fired("IA_Shoot"))
		{
			FlatEngine::GameObject instantiatedObject = FlatEngine::Instantiate("P_IceSpike", transform->GetTruePosition(), -1);
			instantiatedObject.GetRigidBody()->AddForce(Vector2(1, 0), 1000);
			//audio->Play();
		}
		if (mappingContext->Fired("IA_Stab"))
		{			
			//animator->SetAnimationPath("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/animations/A_PlayerStab.json");
			//animator->Play();
			//whipArm->GetAudioComponent()->LoadEffect("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/audio/Swoosh.wav");
			//whipArm->GetAudioComponent()->Play();
			FlatEngine::GameObject instantiatedObject = FlatEngine::Instantiate("P_IceSpike", transform->GetTruePosition(), -1);
			instantiatedObject.GetRigidBody()->AddForce(Vector2(1, 0), 4000);
			//instantiatedObject->GetTransformComponent()->LookAt(Vector2(0, 0));
		}
		if (mappingContext->Fired("IA_Slash"))
		{
			animator->SetAnimationPath("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/animations/A_PlayerSlash.json");
			animator->Play();
		}
		if (characterController != nullptr)
		{
			bool _movingLeft = false;
			bool _movingRight = false;

			if (mappingContext->GetInputAction("IA_MoveLeft").type != 0)
			{
				xDir = -30000;
				_moving = true;
				_movingLeft = true;
				sprite->SetTexture("assets/images/Sprites/owl/walkLeft.png");
			}
			if (mappingContext->GetInputAction("IA_MoveRight").type != 0)
			{
				xDir = 30000;
				_moving = true;
				_movingRight = true;
				sprite->SetTexture("assets/images/Sprites/owl/walkRight.png");
			}

			if (_movingRight && _movingLeft)
			{
				xDir = 0;
				_moving = false;
			}

			//if (mappingContext->GetInputAction("IA_MoveDown").type != 0)
			//{
			//	yDir = -30000;				
			//	_moving = true;
			//}
			//if (mappingContext->GetInputAction("IA_MoveUp").type != 0)
			//{
			//	yDir = 30000;				
			//	_moving = true;
			//}

			if (xDir != 0 || yDir != 0)
			{
				Vector2 normalizedDir = Vector2(xDir, yDir).NormalizeCardinal();				
				characterController->MoveToward(normalizedDir);
			}
		}
	}

	if (!rigidBody->IsGrounded())
	{
		//if (xDir < 0)
		//	sprite->SetTexture("assets/images/Sprites/Player/jumpLeft.png");
		//else if (xDir > 0)
		//	sprite->SetTexture("assets/images/Sprites/Player/jumpRight.png");
		//else
		//	sprite->SetTexture("assets/images/Sprites/Player/jumpStraight.png");
		//sprite->SetOffset(Vector2(10, 15));
	}
	else
	{
		//sprite->SetOffset(Vector2(10, 11));
		if (!_moving)
		{
			sprite->SetTexture("assets/images/Sprites/owl/idle.png");
		}
	}

	//if (mappingContext->GetInputAction("IA_MoveUp").type != 0)
	//	characterController->MoveToward(Vector2(0, 1));
	//if (mappingContext->GetInputAction("IA_MoveDown").type != 0)
	//	characterController->MoveToward(Vector2(0, -1));

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

	characterController->SetMoving(_moving);
}