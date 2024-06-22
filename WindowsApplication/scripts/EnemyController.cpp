#include "EnemyController.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "Animation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "CharacterController.h"

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
		thisObject->GetAnimationComponent()->SetAnimationPath("C:/Users/Dillon Kyle/source/repos/FlatEngine/WindowsApplication/animations/explosion2.json");
		thisObject->GetAnimationComponent()->Play();
	}
}
void DestroySelf(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
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
	animator->AddEventFunction("DestroySelf", DestroySelf);

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
}

void EnemyController::MoveToObject(std::shared_ptr<FlatEngine::GameObject> object)
{
	if (object->HasComponent("Transform") && GetOwner()->HasComponent("CharacterController"))
	{
		Vector2 moveTo = object->GetTransformComponent()->GetTruePosition() - GetOwner()->GetTransformComponent()->GetTruePosition();
		GetOwner()->GetCharacterController()->MoveToward(moveTo.NormalizeCardinal());
		FlatEngine::LogVector2(moveTo.NormalizeCardinal());
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