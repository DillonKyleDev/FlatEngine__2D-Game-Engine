#include "RigidBody.h"
#include "Transform.h"


namespace FlatEngine {
	RigidBody::RigidBody(long myID, long parentID)
	{
		SetType(ComponentTypes::RigidBody);
		SetID(myID);
		SetParentID(parentID);
		mass = 1;
		angularDrag = 1;
		gravity = 1;
		velocity = Vector2(0,0);
		_isMoving = false;
		_isContinious = false;
		_isGrounded = false;
		_isKinematic = false;
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
		_isMoving = toCopy->IsMoving();
		_isContinious = toCopy->IsContinuous();
		_isGrounded = toCopy->IsGrounded();
		_isKinematic = toCopy->IsKinematic();
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
			{ "mass", GetMass()},
			{ "angularDrag", GetAngularDrag() },
			{ "gravity", GetGravity() },
			{ "_isContinious", IsContinuous() },
			{ "_isKinematic", IsKinematic() },
			{ "_isStatic", IsStatic() },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void RigidBody::ApplyGravity()
	{
		if (!_isGrounded && velocity.y <= .2)
			velocity.y -= gravity;
		else if (_isGrounded)
			velocity.y = 0;
	}

	void RigidBody::ApplyVelocity()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		transform->SetPosition(Vector2(position.x + velocity.x, position.y + velocity.y));
	}

	void RigidBody::AddForce(Vector2 direction, float power)
	{

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

	Vector2 RigidBody::GetVelocity()
	{
		return velocity;
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