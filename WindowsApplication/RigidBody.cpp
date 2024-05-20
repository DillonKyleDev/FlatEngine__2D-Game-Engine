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
		pendingForces = Vector2(0, 0);
		acceleration = Vector2(0, 0);
		terminalVelocity = 0.7f;
		windResistance = 0.5f;  // Lower value = more resistance
		friction = 0.93f;  // 1 = no friction. 0 = velocity = 0
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
		acceleration = Vector2(0, 0);
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
		// Add up forces
		ApplyGravity(deltaTime);
		ApplyFriction(deltaTime);

		// Apply them to RigidBody
		if (loadedProject->GetPhysicsSystem() == "Euler")
		{
			CalculateEulerPhysics(deltaTime);
		}
		else if (loadedProject->GetPhysicsSystem() == "Verlet")
		{
			// CalculateVerletPhysics(deltaTime);
		}
	}

	void RigidBody::CalculateEulerPhysics(float deltaTime)
	{
		acceleration = Vector2(pendingForces.x / mass, pendingForces.y / mass);
	}

	void RigidBody::CalculateVerletPhysics(float deltaTime)
	{
		// TODO
	}

	void RigidBody::ApplyPhysics(float deltaTime)
	{
		if (loadedProject->GetPhysicsSystem() == "Euler")
		{
			ApplyEulerPhysics(deltaTime);
		}
		else if (loadedProject->GetPhysicsSystem() == "Verlet")
		{
			// ApplyVerletPhysics(deltaTime);
		}
	}

	void RigidBody::ApplyEulerPhysics(float deltaTime)
	{
		// Then apply to transform
		velocity = Vector2(acceleration.x + (pendingForces.x), acceleration.y + (pendingForces.y));
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		transform->SetPosition(Vector2(position.x + velocity.x, position.y + velocity.y));
	}

	void RigidBody::ApplyVerletPhysics(float deltaTime)
	{
		// TODO
	}

	Vector2 RigidBody::AddVelocity(Vector2 vel, float deltaTime)
	{
		pendingForces.x += vel.x * deltaTime;
		pendingForces.y += vel.y * deltaTime;
		return pendingForces;
	}

	void RigidBody::ApplyGravity(float deltaTime)
	{
		if (gravity > 0)
		{
			if (!_isGrounded && velocity.y > -terminalVelocity)
				pendingForces.y -= gravity * gravityCorrection * deltaTime;
			else if (_isGrounded && pendingForces.y < 0)
				pendingForces.y = 0;
		}
		else if (gravity < 0)
		{
			if (!_isGrounded && velocity.y < terminalVelocity)
				pendingForces.y -= gravity * gravityCorrection * deltaTime;
			else if (_isGrounded && pendingForces.y > 0)
				pendingForces.y = 0;
		}
	}

	void RigidBody::ApplyFriction(float deltaTime)
	{
		// Wind Friction
		if (windResistance * deltaTime < 1)
		{
			Vector2 dampenedVelocity = Vector2(pendingForces.x * (windResistance * deltaTime), pendingForces.y * (windResistance * deltaTime));
			pendingForces = dampenedVelocity;
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
			Vector2 dampenedVelocity = Vector2(pendingForces.x * frictionFactor, pendingForces.y);
			pendingForces = dampenedVelocity;
		}
	}

	void RigidBody::AddForce(Vector2 direction, float power)
	{
		// Normalize the force first, then apply the power factor to the force
		Vector2 addedForce = Vector2(direction.x * power * forceCorrection, direction.y * power * forceCorrection);
		pendingForces.x += addedForce.x;
		pendingForces.y += addedForce.y;
	}

	Vector2 RigidBody::GetNextPosition()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		return Vector2(position.x + velocity.x * gravity * 3, position.y + velocity.y * gravity * 3);
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
		return pendingForces;
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