#include "BlasterRound.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Transform.h"
#include "FlatEngine.h"
#include "Audio.h"


BlasterRound::BlasterRound(long ownerID) : GameScript(ownerID)
{
	SetName("BlasterRound");

	circleCollider = nullptr;	
	startTime = FlatEngine::GetEllapsedGameTimeInSec();
	lifeLength = 3;
}

BlasterRound::~BlasterRound()
{
}

void BlasterRoundOnCollisionEnter(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{	
	std::shared_ptr<FlatEngine::Audio> audio = thisObject->GetAudioComponent();
	audio->Play();
	thisObject->GetCircleCollider()->SetActive(false);
	thisObject->GetTransformComponent()->SetScale(Vector2(1, 1));
	thisObject->GetRigidBody()->SetPendingForces(Vector2(0, 0));
	thisObject->GetRigidBody()->SetVelocity(Vector2(0, 0));
	thisObject->GetAnimationComponent()->Play(FlatEngine::GetEllapsedGameTimeInMs());
}

void BlasterRound::Start()
{
	circleCollider = GetOwner()->GetCircleCollider();
	circleCollider->SetOnCollisionEnter(BlasterRoundOnCollisionEnter);
}

void BlasterRound::Update(float deltaTime)
{
	if (FlatEngine::GetEllapsedGameTimeInSec() - startTime >= lifeLength)
		FlatEngine::DeleteGameObject(GetOwner()->GetID());
}