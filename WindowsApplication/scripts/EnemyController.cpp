#include "EnemyController.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"


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
	FlatEngine::LogString(collidedWith->GetName());
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
	if (GetOwner()->GetFirstChild()->GetName() == "WhipArm")
		whipArm = GetOwner()->GetFirstChild();
	animator = whipArm->GetAnimationComponent();

	//boxCollider->SetOnCollisionEnter(OnCollisionEnter);
	//for (std::shared_ptr<FlatEngine::CircleCollider> circleCollider : circleColliders)
	//	circleCollider->SetOnCollisionEnter(OnCollisionEnter);

	//if (!GetOwner()->HasComponent("CompositeCollider"))
	//	compositeCollider = GetOwner()->AddCompositeColliderComponent();
	//else
	//	compositeCollider = GetOwner()->GetCompositeCollider();
	//
	//compositeCollider->SetOnCollisionEnter(OnCollisionEnter);
}

void EnemyController::Update(float deltaTime)
{

}