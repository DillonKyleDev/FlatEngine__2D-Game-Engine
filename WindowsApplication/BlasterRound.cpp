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

	boxCollider = nullptr;	
	startTime = FlatEngine::GetEllapsedGameTimeInSec();
	lifeLength = 2;
}

BlasterRound::~BlasterRound()
{
}

void BlasterRoundOnCollisionEnter(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{	
	std::shared_ptr<FlatEngine::Audio> audio = thisObject->AddAudioComponent();
	audio->SetPath("assets/audio/explosion.wav");
	audio->SetIsMusic(false);
	audio->Play();
	thisObject->GetBoxCollider()->SetActive(false);
	thisObject->GetTransformComponent()->SetScale(Vector2(1, 1));
	thisObject->GetRigidBody()->SetPendingForces(Vector2(0, 0));
	thisObject->GetRigidBody()->SetVelocity(Vector2(0, 0));
	thisObject->GetAnimationComponent()->Play(FlatEngine::GetEllapsedGameTimeInMs());
}

void BlasterRound::Start()
{
	boxCollider = GetOwner()->GetBoxCollider();	
	boxCollider->SetOnCollisionEnter(BlasterRoundOnCollisionEnter);	
}

void BlasterRound::Update(float deltaTime)
{
	if (FlatEngine::GetEllapsedGameTimeInSec() - startTime >= lifeLength)
		FlatEngine::DeleteGameObject(GetOwner()->GetID());
}