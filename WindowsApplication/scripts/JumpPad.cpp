#include "JumpPad.h"
#include "../FlatEngine.h"
#include "../BoxCollider.h"
#include "../RigidBody.h"
#include "../Transform.h"


JumpPad::JumpPad()
{
	this->SetName("JumpPad");
}

JumpPad::~JumpPad()
{
}

void JumpPadOnCollisionEnter(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	FlatEngine::LogString("Collided!");
	std::shared_ptr<FlatEngine::RigidBody> collidedRigidBody = collidedWith->GetRigidBody();
	std::shared_ptr<FlatEngine::Transform> myTransform = thisObject->GetTransformComponent();
	std::shared_ptr<FlatEngine::Transform> collidedTransform = collidedWith->GetTransformComponent();
	Vector2 myPosition = myTransform->GetTruePosition();
	Vector2 collidedPosition = collidedTransform->GetTruePosition();
	Vector2 towardCollidedWith = Vector2(collidedPosition.x - myPosition.x, collidedPosition.y - myPosition.y);
	Vector2 pendingForces = collidedRigidBody->GetPendingForces();
	collidedRigidBody->SetPendingForces(Vector2(pendingForces.x, 0));
	collidedRigidBody->AddForce(Vector2(0, 1), 1);
}

void JumpPad::Start()
{	
	boxCollider = GetOwner()->GetBoxCollider();
	//boxCollider->SetOnCollisionEnter(JumpPadOnCollisionEnter);
	boxCollider->SetOnColliding(JumpPadOnCollisionEnter);
}

void JumpPad::Update(int deltaTime)
{
}