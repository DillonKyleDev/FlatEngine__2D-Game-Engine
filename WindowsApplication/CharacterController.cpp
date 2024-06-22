#include "CharacterController.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Vector2.h"


namespace FlatEngine {
	CharacterController::CharacterController(long myID, long parentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(myID);
		SetParentID(parentID);
		maxAcceleration = 1;
		maxSpeed = 0.1f;
		airControl = 0.2f;
		speedCorrection = 1;
		_isMoving = false;
	}

	// Copy constructor
	CharacterController::CharacterController(std::shared_ptr<CharacterController> toCopy, long newParentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(FlatEngine::GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		maxAcceleration = toCopy->maxAcceleration;
		maxSpeed = toCopy->maxSpeed;
		airControl = toCopy->airControl;
		speedCorrection = toCopy->speedCorrection;
		_isMoving = toCopy->_isMoving;
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
			{ "maxAcceleration", maxAcceleration },
			{ "maxSpeed", maxSpeed },
			{ "airControl", airControl }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CharacterController::MoveToward(Vector2 direction)
	{
		if (GetParent()->HasComponent("Transform") && GetParent()->HasComponent("RigidBody"))
		{
			Vector2 pendingForces = Vector2(0, 0);
			std::shared_ptr<FlatEngine::RigidBody> rigidBody = GetParent()->GetRigidBody();
			std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
			float gravity = 0;

			if (rigidBody != nullptr)
			{
				pendingForces = rigidBody->GetPendingForces();
				gravity = rigidBody->GetGravity();
			}
			else
				LogString("CharacterController.cpp - RigidBody == nullptr");

			// If the object has not hit max speed in negative or positive direction
			if ((pendingForces.x >= 0 && pendingForces.x < maxSpeed) || (pendingForces.x <= 0 && pendingForces.x > -maxSpeed) ||
				// If velocity exceeds positive max speed but x direction is negative
				((pendingForces.x >= maxSpeed && direction.x * maxAcceleration < 0) ||
				// If velocity exceeds negative max speed but x direction is positive
				(pendingForces.x <= -maxSpeed && direction.x * maxAcceleration > 0)))
			{
				if (rigidBody != nullptr && (gravity != 0 && !rigidBody->IsGrounded()))
					rigidBody->AddVelocity(Vector2(direction.x * maxAcceleration * airControl, direction.y * maxAcceleration * airControl));
				else if (rigidBody != nullptr)
				{
					rigidBody->AddVelocity(Vector2(direction.x * maxAcceleration, direction.y * maxAcceleration));
				}
				_isMoving = true;
			}

			if (direction.x == 0)
				_isMoving = false;
		}
	}

	void CharacterController::SetMaxAcceleration(float newMaxAcceleration)
	{
		maxAcceleration = newMaxAcceleration;
	}

	float CharacterController::GetMaxAcceleration()
	{
		return maxAcceleration;
	}

	void CharacterController::SetMaxSpeed(float newMaxSpeed)
	{
		maxSpeed = newMaxSpeed;
	}

	float CharacterController::GetMaxSpeed()
	{
		return maxSpeed;
	}

	void CharacterController::SetAirControl(float newAirControl)
	{
		airControl = newAirControl;
	}

	float CharacterController::GetAirControl()
	{
		return airControl;
	}

	void CharacterController::SetMoving(bool _newIsMoving)
	{
		_isMoving = _newIsMoving;
	}

	bool CharacterController::IsMoving()
	{
		return _isMoving;
	}
}