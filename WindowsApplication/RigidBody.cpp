#include "RigidBody.h"
#include "Transform.h"
#include "CharacterController.h"


namespace FlatEngine {
	RigidBody::RigidBody(long myID, long parentID)
	{
		SetType(ComponentTypes::RigidBody);
		SetID(myID);
		SetParentID(parentID);
		mass = 1;
		angularDrag = 1;
		gravity = 1;
		gravityCorrection = 0.001f;
		velocity = Vector2(0, 0);
		pendingVelocity = Vector2(0, 0);
		terminalVelocity = -1.0f;
		windResistance = 0.5f;  // Lower value = more resistance
		friction = 0.83f;  // 1 = no friction. 0 = velocity = 0
		forceCorrection = 0.01f;
		_isMoving = false;
		_isContinious = false;
		_isGrounded = false;
		_isKinematic = false;
		_isStatic = false;
	}

	RigidBody::RigidBody(std::shared_ptr<RigidBody> toCopy, long newParentID)
	{
		SetType(ComponentTypes::RigidBody);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		mass = toCopy->GetMass();
		angularDrag = toCopy->GetAngularDrag();
		gravity = toCopy->GetGravity();
		velocity = toCopy->GetVelocity();
		terminalVelocity = toCopy->GetTerminalVelocity();
		windResistance = toCopy->windResistance;
		friction = toCopy->friction;
		forceCorrection = forceCorrection;
		_isMoving = toCopy->IsMoving();
		_isContinious = toCopy->IsContinuous();
		_isGrounded = toCopy->IsGrounded();
		_isKinematic = toCopy->IsKinematic();
		_isStatic = toCopy->IsStatic();
	}

	RigidBody::~RigidBody()
	{
	}

	std::string RigidBody::GetData()
	{
		json jsonData = {
			{ "type", "RigidBody" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "mass", mass},
			{ "angularDrag", angularDrag },
			{ "gravity", gravity },
			{ "terminalVelocity", terminalVelocity },
			{ "windResistance", windResistance },
			{ "_isContinious", _isContinious },
			{ "_isKinematic", _isKinematic },
			{ "_isStatic", _isStatic },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void RigidBody::CalculatePhysics(float deltaTime)
	{
		ApplyGravity(deltaTime);
		ApplyFriction(deltaTime);
	}

	void RigidBody::ApplyPhysics()
	{
		// Then apply to transform
		ApplyVelocity();
	}

	Vector2 RigidBody::AddVelocity(Vector2 vel, float deltaTime)
	{
		pendingVelocity.x += vel.x * deltaTime;
		pendingVelocity.y += vel.y * deltaTime;
		return pendingVelocity;
	}

	void RigidBody::ApplyGravity(float deltaTime)
	{
		if (!_isGrounded && velocity.y > terminalVelocity)
			pendingVelocity.y -= gravity * gravityCorrection * deltaTime;
		else if (_isGrounded && pendingVelocity.y < 0)
			pendingVelocity.y = 0;
	}

	// Apply the accumulated velocity to the actual transform
	void RigidBody::ApplyVelocity()
	{
		LogVector2(pendingVelocity, "Pending Velocity: ");
		velocity = pendingVelocity;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		transform->SetPosition(Vector2(position.x + velocity.x, position.y + velocity.y));
	}

	void RigidBody::ApplyFriction(float deltaTime)
	{
		// Wind Friction
		if (windResistance * deltaTime < 1)
		{
			Vector2 dampenedVelocity = Vector2(pendingVelocity.x * (windResistance * deltaTime), pendingVelocity.y * (windResistance * deltaTime));
			pendingVelocity = dampenedVelocity;
		}

		// Get Character Controller for _isMoving
		std::shared_ptr<FlatEngine::CharacterController> characterController = GetParent()->GetCharacterController();
		bool _isMoving = false;
		if (characterController != nullptr)
			_isMoving = characterController->IsMoving();

		// Ground Friction
		if (!_isMoving && _isGrounded)
		{
			float frictionFactor = friction * deltaTime;
			if (frictionFactor > 1)
				frictionFactor = friction;
			Vector2 dampenedVelocity = Vector2(pendingVelocity.x * frictionFactor, pendingVelocity.y);
			pendingVelocity = dampenedVelocity;
		}
	}

	void RigidBody::AddForce(Vector2 direction, float power)
	{
		// Normalize the force first, then apply the power factor to the force
		Vector2 addedForce = Vector2(direction.x * power * forceCorrection, direction.y * power * forceCorrection);
		pendingVelocity.x += addedForce.x;
		pendingVelocity.y += addedForce.y;
		LogFloat(pendingVelocity.y, "Vel y: ");
	}

	Vector2 RigidBody::GetNextPosition()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		return Vector2(position.x + pendingVelocity.x * 1.9, position.y + pendingVelocity.y * 1.9);
	}

	void RigidBody::Move(Vector2 position)
	{
		// At some point add some physics into this to make it actually move the character with force
		// and take into consideration weight, momentum, etc...
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		transform->SetPosition(position);
	}

	void RigidBody::SetMass(float newMass)
	{
		mass = newMass;
	}

	float RigidBody::GetMass()
	{
		return mass;
	}

	void RigidBody::SetAngularDrag(float newAngularDrag)
	{
		angularDrag = newAngularDrag;
	}

	float RigidBody::GetAngularDrag()
	{
		return angularDrag;
	}

	void RigidBody::SetGravity(float newGravity)
	{
		gravity = newGravity;
	}

	float RigidBody::GetGravity()
	{
		return gravity;
	}

	void RigidBody::SetVelocity(Vector2 newVelocity)
	{
		velocity = newVelocity;
	}

	void RigidBody::SetTerminalVelocity(float newTerminalVelocity)
	{
		terminalVelocity = newTerminalVelocity;
	}

	float RigidBody::GetTerminalVelocity()
	{
		return terminalVelocity;
	}

	Vector2 RigidBody::GetVelocity()
	{
		return velocity;
	}

	Vector2 RigidBody::GetPendingVelocity()
	{
		return pendingVelocity;
	}

	void RigidBody::SetIsMoving(bool _moving)
	{
		_isMoving = _moving;
	}

	bool RigidBody::IsMoving()
	{
		return _isMoving;
	}

	void RigidBody::SetIsContinuous(bool _continuous)
	{
		_isContinious = _continuous;
	}

	bool RigidBody::IsContinuous()
	{
		return _isContinious;
	}

	void RigidBody::SetIsStatic(bool _static)
	{
		_isStatic = _static;
	}

	bool RigidBody::IsStatic()
	{
		return _isStatic;
	}

	void RigidBody::SetIsGrounded(bool _grounded)
	{
		_isGrounded = _grounded;
	}

	bool RigidBody::IsGrounded()
	{
		return _isGrounded;
	}

	void RigidBody::SetIsKinematic(bool _kinematic)
	{
		_isKinematic = _kinematic;
	}

	bool RigidBody::IsKinematic()
	{
		return _isKinematic;
	}
}