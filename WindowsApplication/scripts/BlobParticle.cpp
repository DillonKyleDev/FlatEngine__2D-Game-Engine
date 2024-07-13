#include "BlobParticle.h"
#include "../Animation.h"
#include "../FlatEngine.h"
#include "../GameObject.h"

BlobParticle::BlobParticle(long ownerID) : GameScript(ownerID)
{
	SetName("BlobParticle");
	m_TestObject = FlatEngine::GameObject();
}

BlobParticle::~BlobParticle()
{
}

void BlobParticle::Start()
{
	GetOwner()->GetAnimationComponent()->AddEventFunction("DestroySelfAndParent", FlatEngine::DestroySelfAndParent);
}

void BlobParticle::Update(float deltaTime)
{
}