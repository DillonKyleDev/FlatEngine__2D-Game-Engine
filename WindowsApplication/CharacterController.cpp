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
		Vector2 pendingForces = Vector2(0, 0);
		std::shared_ptr<FlatEngine::RigidBody> rigidBody = GetParent()->GetRigidBody();
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		float gravity = 0;
		float normalizedX = direction.x / 32760;
		float normalizedY = direction.y / 32760;

		if (rigidBody != nullptr)
		{
			pendingForces = rigidBody->GetPendingForces();
			gravity = rigidBody->GetGravity();
		}
		else
			LogString("CharacterController.cpp - RigidBody == nullptr");


		if (normalizedX > 1)
			normalizedX = 1;
		if (normalizedX < -1)
			normalizedX = -1;
		if (normalizedY > 1)
			normalizedY = 1;
		if (normalizedY < -1)
			normalizedY = -1;


		// If the object has not hit max speed in negative or positive direction
		if (pendingForces.x >= 0 || pendingForces.x <= 0 && pendingForces.x > (maxSpeed * -1) ||
			// If velocity exceeds positive max speed but x direction is negative
			(normalizedX * maxAcceleration < 0) ||
			// If velocity exceeds negative max speed but x direction is positive
			(normalizedX * maxAcceleration > 0) && normalizedX != 0)
		{
			if (rigidBody != nullptr && (gravity != 0 && !rigidBody->IsGrounded()))
				rigidBody->AddVelocity(Vector2(normalizedX * maxAcceleration * airControl, normalizedY * maxAcceleration * airControl));
			else if (rigidBody != nullptr)
				rigidBody->AddVelocity(Vector2(normalizedX * maxAcceleration, normalizedY * maxAcceleration));
			_isMoving = true;
		}		
		
		if (normalizedX == 0)
			_isMoving = false;
	}

	void CharacterController::MoveTo(Vector2 location)
	{
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