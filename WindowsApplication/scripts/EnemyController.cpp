#include "EnemyController.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "Animation.h"


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

	compositeCollider = nullptr;
}

EnemyController::~EnemyController()
{
}

void OnCollisionEnter(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	//FlatEngine::LogString(collidedWith->GetName());
}

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

void EnemyController::Start()
{
	FlatEngine::LogString("EnemyController instantiated on: " + GetOwner()->GetName());	
	health = std::static_pointer_cast<Health>(GetOwner()->GetGameScriptByName("Health"));
	boxCollider = GetOwner()->GetBoxCollider();
	circleColliders = GetOwner()->GetCircleColliders();
	rigidBody = GetOwner()->GetRigidBody();
	transform = GetOwner()->GetTransformComponent();
	sprite = GetOwner()->GetSpriteComponent();
	audio = GetOwner()->GetAudioComponent();
	//audio->SetPath("assets/audio/lazerFire.wav");
	//audio->SetIsMusic(false);
	animator = GetOwner()->GetAnimationComponent();
	animator->AddEventFunction("DestroySelf", DestroySelf);

	//boxCollider->SetOnCollisionEnter(OnCollisionEnter);
	//for (std::shared_ptr<FlatEngine::CircleCollider> circleCollider : circleColliders)
	//	circleCollider->SetOnCollisionEnter(OnCollisionEnter);

	//if (!GetOwner()->HasComponent("CompositeCollider"))
	//	compositeCollider = GetOwner()->AddCompositeColliderComponent();
	//else
	//	compositeCollider = GetOwner()->GetCompositeCollider();
	//
	//compositeCollider->SetOnCollisionEnter(OnCollisionEnter);

	// HEALTH
	health->SetOnTakeDamage(OnTakeDamage);
	health->SetOnDeath(OnDeath);
}

void EnemyController::Update(float deltaTime)
{

}