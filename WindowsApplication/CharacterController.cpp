#include "CharacterController.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "RigidBody.h"
#include "BoxCollider.h"


namespace FlatEngine {
	CharacterController::CharacterController(long myID, long parentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(myID);
		SetParentID(parentID);
		walkSpeed = 1;
		runSpeed = 2;
		maxSpeed = 0.01f;
		airControl = 0.5f;
		speedCorrection = 0.001f;
		_isMoving = false;
		_isGrounded = false;
	}

	// Copy constructor
	CharacterController::CharacterController(std::shared_ptr<CharacterController> toCopy, long newParentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(FlatEngine::GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		walkSpeed = toCopy->GetWalkSpeed();
		runSpeed = toCopy->GetWalkSpeed();
		speedCorrection = 0.001f;
		_isMoving = toCopy->IsMoving();
		_isGrounded = toCopy->IsGrounded();
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
			{ "_isActive", IsActive() },
			{ "walkSpeed", walkSpeed },
			{ "runSpeed", runSpeed },
			{ "_isMoving", _isMoving },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CharacterController::MoveToward(Vector2 direction)
	{
		std::shared_ptr<FlatEngine::RigidBody> rigidBody = GetParent()->GetRigidBody();
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

		Vector2 pendingForces = rigidBody->GetPendingForces();
		// If the object has not hit max speed in negative or positive direction
		if (pendingForces.x >= 0 && pendingForces.x < maxSpeed || pendingForces.x <= 0 && pendingForces.x > (maxSpeed * -1) ||
			// If velocity exceeds positive max speed but x direction is negative
			(pendingForces.x >= maxSpeed && normalizedX * walkSpeed * speedCorrection < 0) ||
			// If velocity exceeds negative max speed but x direction is positive
			(pendingForces.x <= (maxSpeed * -1) && normalizedX * walkSpeed * speedCorrection > 0) && normalizedX != 0)
		{
			if (!rigidBody->IsGrounded())
				rigidBody->AddVelocity(Vector2(normalizedX * walkSpeed * airControl * speedCorrection, 0));
			else
				rigidBody->AddVelocity(Vector2(normalizedX * walkSpeed * speedCorrection, 0));
			_isMoving = true;
		}		
		
		if (normalizedX == 0)
			_isMoving = false;
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

	float CharacterController::GetMaxSpeed()
	{
		return maxSpeed;
	}

	void CharacterController::SetRunSpeed(float speed)
	{
		runSpeed = speed;
	}

	float CharacterController::GetRunSpeed()
	{
		return runSpeed;
	}

	void CharacterController::SetMoving(bool _newIsMoving)
	{
		_isMoving = _newIsMoving;
	}

	bool CharacterController::IsMoving()
	{
		return _isMoving;
	}

	void CharacterController::SetIsGrounded(bool _grounded)
	{
		_isGrounded = _grounded;
	}

	bool CharacterController::IsGrounded()
	{
		return _isGrounded;
	}
}