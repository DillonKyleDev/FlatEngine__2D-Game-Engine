#include "CharacterController.h"
#include "FlatEngine.h"
#include "Transform.h"


namespace FlatEngine {
	CharacterController::CharacterController(long myID, long parentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(myID);
		SetParentID(parentID);
		walkSpeed = 1;
		runSpeed = 2;
		gravity = 1;
		_isMoving = false;
		velocity = 0;
	}

	// Copy constructor
	CharacterController::CharacterController(std::shared_ptr<CharacterController> toCopy, long newParentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(FlatEngine::GetNextComponentID());
		SetParentID(newParentID);
		walkSpeed = 1;
		runSpeed = 2;
		gravity = 1;
		_isMoving = false;
		velocity = 0;
	}

	CharacterController::~CharacterController()
	{
	}

	std::string CharacterController::GetData()
	{
		json jsonData = {
			{ "type", "CharacterController" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "walkSpeed", walkSpeed },
			{ "runSpeed", runSpeed },
			{ "gravity", gravity },
			{ "_isMoving", _isMoving },
			{ "velocity", velocity }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CharacterController::MoveToward(Vector2 direction)
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();

		float normalizedX = direction.x / 32760;
		float normalizedY = direction.y / 32760;

		if (normalizedX > 1)
			normalizedX = 1;
		if (normalizedX < -1)
			normalizedX = -1;
		if (normalizedY > 1)
			normalizedY = 1;
		if (normalizedY < -1)
			normalizedY = -1;

		LogFloat(normalizedX, "normX: ");
		LogFloat(normalizedY, "normY: ");

		transform->SetPosition(Vector2(transform->GetPosition().x + normalizedX * walkSpeed, transform->GetPosition().y));
		transform->SetPosition(Vector2(transform->GetPosition().x, transform->GetPosition().y - normalizedY * walkSpeed));
	}

	void CharacterController::MoveTo(Vector2 location)
	{
	}

	void CharacterController::SetWalkSpeed(float speed)
	{
		walkSpeed = speed;
	}

	float CharacterController::GetWalkSpeed()
	{
		return walkSpeed;
	}

	void CharacterController::SetRunSpeed(float speed)
	{
		runSpeed = speed;
	}

	float CharacterController::GetRunSpeed()
	{
		return runSpeed;
	}

	void CharacterController::SetGravity(float newGravity)
	{
		gravity = newGravity;
	}

	float CharacterController::GetGravity()
	{
		return gravity;
	}

	void CharacterController::SetMoving(bool _newIsMoving)
	{
		_isMoving = _newIsMoving;
	}

	bool CharacterController::IsMoving()
	{
		return _isMoving;
	}

	void CharacterController::SetVelocity(float vel)
	{
		velocity = vel;
	}

	float CharacterController::GetVelocity()
	{
		return velocity;
	}
}