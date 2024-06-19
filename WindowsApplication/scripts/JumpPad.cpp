#include "JumpPad.h"
#include "../FlatEngine.h"
#include "../BoxCollider.h"
#include "../RigidBody.h"
#include "../Transform.h"
#include "Audio.h"


JumpPad::JumpPad(long ownerID) : GameScript(ownerID)
{
	this->SetName("JumpPad");
	boxCollider = nullptr;
}

JumpPad::~JumpPad()
{
}

void JumpPadOnCollisionEnter(std::shared_ptr<FlatEngine::GameObject> thisObject, std::shared_ptr<FlatEngine::GameObject> collidedWith)
{
	if (collidedWith->HasComponent("RigidBody"))
	{
		std::shared_ptr<FlatEngine::Audio> audio = thisObject->GetAudioComponent();
		if (audio != nullptr)
		{
			audio->SetPath("assets/audio/bounce.wav");
			audio->SetIsMusic(false);
			audio->Play();
		}

		std::shared_ptr<FlatEngine::RigidBody> collidedRigidBody = collidedWith->GetRigidBody();
		std::shared_ptr<FlatEngine::Transform> myTransform = thisObject->GetTransformComponent();
		std::shared_ptr<FlatEngine::Transform> collidedTransform = collidedWith->GetTransformComponent();
		Vector2 myPosition = myTransform->GetTruePosition();
		Vector2 collidedPosition = collidedTransform->GetTruePosition();
		Vector2 towardCollidedWith = Vector2(collidedPosition.x - myPosition.x, collidedPosition.y - myPosition.y);
		Vector2 pendingForces = collidedRigidBody->GetPendingForces();
		collidedRigidBody->SetPendingForces(Vector2(pendingForces.x, 0));
		collidedRigidBody->AddForce(Vector2(0, 1), 2000);
	}
}

void JumpPad::Start()
{	
	boxCollider = GetOwner()->GetBoxCollider();	
	boxCollider->SetOnCollisionEnter(JumpPadOnCollisionEnter);
}

void JumpPad::Update(float deltaTime)
{
}