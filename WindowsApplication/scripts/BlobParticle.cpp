#include "BlobParticle.h"
#include "../Animation.h"
#include "../FlatEngine.h"
#include "../GameObject.h"
#include "../Transform.h"

#include "math.h"


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
	//GetOwner()->GetAnimationComponent()->AddEventFunction("DestroySelfAndParent", FlatEngine::DestroySelfAndParent);
}

void BlobParticle::Update(float deltaTime)
{
	std::shared_ptr<FlatEngine::GameObject> grabObject = FlatEngine::GetObjectByName("Grab");
	std::shared_ptr<FlatEngine::Transform> grabTransform = grabObject->GetTransformComponent();
	std::shared_ptr<FlatEngine::Transform> myTransform = GetOwner()->GetTransformComponent();
	float rotation = grabTransform->GetRotation();
	GetOwner()->GetRigidBody()->_isCircular = true;
	Vector2 attachedPos = grabTransform->GetTruePosition();
	Vector2 myPos = GetOwner()->GetTransformComponent()->GetTruePosition();
	float radius = 5;

	float cos_a = radius * cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
	float sin_a = radius * sinf(rotation * 2.0f * (float)M_PI / 360.0f);

	FlatEngine::LogFloat(cos_a, "Cos a: ");
	FlatEngine::LogFloat(sin_a, "Sin a: ");



	myTransform->SetPosition(Vector2(cos_a, sin_a));

	//// Calculate center distance with pythag
	float rise = attachedPos.y - myPos.y;
	float run = attachedPos.x - myPos.x;
	float distance = std::sqrt((rise * rise) + (run * run));
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