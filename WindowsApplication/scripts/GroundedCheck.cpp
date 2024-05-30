#include "GroundedCheck.h"
#include "../FlatEngine.h"
#include "../BoxCollider.h"
#include "../CharacterController.h"


GroundedCheck::GroundedCheck()
{
	this->SetName("GroundedCheck");
	collider = nullptr;
	controller = nullptr;
}

GroundedCheck::~GroundedCheck()
{
}


void GroundedCheck::Start()
{
	collider = GetOwner()->GetBoxCollider();
	std::shared_ptr<FlatEngine::GameObject> parent = GetOwner()->GetParent();
	if (parent != nullptr)
		controller = parent->GetCharacterController();
}

void GroundedCheck::Update(float deltaTime)
{
	if (collider != nullptr && collider->IsColliding() && controller != nullptr)
		controller->SetIsGrounded(true);
}