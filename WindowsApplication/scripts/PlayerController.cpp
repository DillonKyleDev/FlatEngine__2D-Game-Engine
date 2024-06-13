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

#include "GameObject.h"
#include "Transform.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Sprite.h"
#include "BlasterRound.h"
#include "Animation.h"
#include "Audio.h"


PlayerController::PlayerController(long ownerID) : GameScript(ownerID)
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
	bool _isGrounded = false;
	static int xDir = 0;
	static int yDir = 0;
	Vector2 velocity = Vector2(0, 0);

	if (rigidBody != nullptr)
	{
		velocity = rigidBody->GetVelocity();
		_isGrounded = rigidBody->IsGrounded();
	}

	if (mappingContext != nullptr)
	{
		if (mappingContext->Fired("IA_Jump"))
		{
			if (_isGrounded && velocity.y < 0.001f && velocity.y > -0.001f)
			{
				rigidBody->AddForce(Vector2(0, 1), 1300);
			}
		}
		if (mappingContext->Fired("IA_Shoot"))
		{
			//FlatEngine::Instantiate("BlasterRound", transform->GetTruePosition());
			std::shared_ptr<FlatEngine::GameObject> instantiatedObject = FlatEngine::CreateGameObject(-1);
			instantiatedObject->SetName("BlasterRound");
			instantiatedObject->SetTag("Projectile", true);
			instantiatedObject->SetTag("IgnorePlayer", true);
			std::shared_ptr<FlatEngine::Transform> newTransform = instantiatedObject->AddTransformComponent();
			std::shared_ptr<FlatEngine::RigidBody> rigidBody = instantiatedObject->AddRigidBodyComponent();
			std::shared_ptr<FlatEngine::BoxCollider> boxCollider = instantiatedObject->AddBoxColliderComponent();
			std::shared_ptr<FlatEngine::Sprite> sprite = instantiatedObject->AddSpriteComponent();
			std::shared_ptr<FlatEngine::ScriptComponent> script = instantiatedObject->AddScriptComponent();
			std::shared_ptr<FlatEngine::Animation> animation = instantiatedObject->AddAnimationComponent();
			std::shared_ptr<FlatEngine::Audio> audio = instantiatedObject->AddAudioComponent();
			animation->SetAnimationPath("animations/explosion2.json");
			newTransform->SetOrigin(Vector2(0, 0));
			newTransform->SetPosition(transform->GetTruePosition());
			newTransform->SetScale(Vector2(0.25f, 0.25f));
			rigidBody->SetGravity(0);
			rigidBody->SetMass(0.1f);
			rigidBody->SetFallingGravity(0);
			rigidBody->AddForce(Vector2(1, 0), 150);
			rigidBody->SetFriction(1);
			rigidBody->SetWindResistance(1);
			boxCollider->SetIsSolid(false);
			script->SetAttachedScript("BlasterRound");
			sprite->SetTexture("assets/images/sprites/BlasterRound.png");
			sprite->SetRenderOrder(4);
			audio->SetPath("assets/audio/lazerFire.wav");			
			audio->SetIsMusic(false);
			audio->Play();
			std::shared_ptr<BlasterRound> blasterRoundScript = std::make_shared<BlasterRound>(script->GetID());
			FlatEngine::gameLoop->UpdateActiveColliders();
			FlatEngine::gameLoop->UpdateActiveRigidBodies();
			FlatEngine::gameLoop->AddScript(instantiatedObject, script, blasterRoundScript);
		}
		if (characterController != nullptr)
		{
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
		}
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