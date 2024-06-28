#include "EnemyController.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "Animation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "Animation.h"
#include "Sprite.h"
#include <random>


// Event functions
void OnTakeDamage(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> damagedBy, float damageAmount)
{
	if (thisObject->HasComponent("Animation"))
	{
		thisObject->GetAnimationComponent()->Play();
	}
}
void OnDeath(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> killedBy)
{
	if (thisObject->HasComponent("Animation"))
	{
		//thisObject->GetAnimationComponent()->SetAnimationPath("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/animations/explosion2.json");
		//thisObject->GetAnimationComponent()->Play();
	}

	for (int i = 0; i < 10; i++)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dir(0, 200); // distribution in range [-1, 1]

		std::uniform_int_distribution<std::mt19937::result_type> pow(500, 3000); // distribution in range [-1, 1]
		Vector2 direction = Vector2(dir(rng), dir(rng));
		if (direction.x >= 100)
			direction.x *= -.01;
		else
			direction.x *= .01;
		if (direction.y >= 100)
			direction.y *= -.01;
		else
			direction.y *= .01;

		float power = pow(rng);

		std::shared_ptr<FlatEngine::GameObject> blobParticle = FlatEngine::Instantiate("P_BlobParticle", thisObject->GetTransformComponent()->GetTruePosition());
		std::shared_ptr<FlatEngine::RigidBody> blobRigidBody = blobParticle->GetRigidBody();
		blobParticle->GetFirstChild()->GetAnimationComponent()->Play();

		blobRigidBody->AddForce(direction, power);
	}

	FlatEngine::DeleteGameObject(thisObject->GetID());
}
void OnEnterLineOfSight(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	if (collidedWith->HasTag("Player"))
	{
		// Get this script instance
		std::shared_ptr<EnemyController> enemyController = std::static_pointer_cast<EnemyController>(thisObject->GetParent()->GetGameScriptByName("EnemyController"));
		enemyController->SetSeesPlayer(true);
	}
}
void OnLeaveLineOfSight(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	if (collidedWith->HasTag("Player"))
	{
		// Get this script instance
		std::shared_ptr<EnemyController> enemyController = std::static_pointer_cast<EnemyController>(thisObject->GetGameScriptByName("EnemyController"));
		enemyController->StartChaseTimeout();
	}
}


EnemyController::EnemyController(long ownerID) : GameScript(ownerID)
{
	SetName("EnemyController");
	health = nullptr;
	boxCollider = nullptr;
	circleColliders = std::vector<std::shared_ptr<FlatEngine::CircleCollider>>();
	rigidBody = nullptr;
	transform = nullptr;
	animator = nullptr;
	sprite = nullptr;
	audio = nullptr;
	whipArm = nullptr;
	lineOfSight = nullptr;

	compositeCollider = nullptr;

	_seesPlayer = false;
	chaseTimeout = 5;
	chaseTimerStarted = 0;
	maxHealth = 100;
	attackDamage = 10;
}

EnemyController::~EnemyController()
{
}

void EnemyController::Start()
{
	health = std::static_pointer_cast<Health>(GetOwner()->GetGameScriptByName("Health"));
	boxCollider = GetOwner()->GetBoxCollider();
	circleColliders = GetOwner()->GetCircleColliders();
	rigidBody = GetOwner()->GetRigidBody();
	transform = GetOwner()->GetTransformComponent();
	sprite = GetOwner()->GetSpriteComponent();
	audio = GetOwner()->GetAudioComponent();
	animator = GetOwner()->GetAnimationComponent();
	animator->AddEventFunction("DestroySelf", FlatEngine::DestroySelf);

	// Health
	health->SetOnTakeDamage(OnTakeDamage);
	health->SetOnDeath(OnDeath);

	// Line of sight BoxCollider
	lineOfSight = GetOwner()->FindChildByName("LineOfSight");
	if (lineOfSight != nullptr && lineOfSight->HasComponent("BoxCollider"))
	{
		lineOfSight->GetBoxCollider()->SetOnCollisionEnter(OnEnterLineOfSight);
	}
}

void EnemyController::Update(float deltaTime)
{
	if (_seesPlayer)
	{
		MoveToObject(FlatEngine::GetPlayerObject());

		if (chaseTimerStarted != 0)
		{

		}
	}
	else if (GetOwner()->GetSpriteComponent()->GetPath() != "C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/idle.png")
		GetOwner()->GetSpriteComponent()->SetTexture("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/idle.png");
}

void EnemyController::MoveToObject(std::shared_ptr<FlatEngine::GameObject> object)
{
	if (object->HasComponent("Transform") && GetOwner()->HasComponent("CharacterController"))
	{
		std::shared_ptr<FlatEngine::Sprite> sprite = GetOwner()->GetSpriteComponent();
		Vector2 moveTo = object->GetTransformComponent()->GetTruePosition() - GetOwner()->GetTransformComponent()->GetTruePosition();
		GetOwner()->GetCharacterController()->MoveToward(moveTo.NormalizeCardinal());
		FlatEngine::LogVector2(moveTo.NormalizeCardinal());
		if (moveTo.x < 0 && sprite->GetPath() != "C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/walkLeft.png")
			sprite->SetTexture("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/walkLeft.png");
		else if (moveTo.x > 0 && sprite->GetPath() != "C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/walkRight.png")
			sprite->SetTexture("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/assets/images/Sprites/owl/walkRight.png");
	}
}

void EnemyController::SetSeesPlayer(bool _newSeesPlayer)
{
	_seesPlayer = _newSeesPlayer;
}

void EnemyController::StartChaseTimeout()
{
	chaseTimerStarted = FlatEngine::GetEngineTime();
}

void EnemyController::StopChaseTimeout()
{
	chaseTimerStarted = 0;
}