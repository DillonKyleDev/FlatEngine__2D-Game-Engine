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

void BlobParticle::LogObjectName()
{
	FlatEngine::LogString("Object Name: " + m_TestObject.GetName());
}

FlatEngine::GameObject& BlobParticle::GetTestObject()
{
	return m_TestObject;
}
