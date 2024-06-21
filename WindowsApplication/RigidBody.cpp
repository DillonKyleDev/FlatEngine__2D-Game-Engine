#include "RigidBody.h"
#include "Transform.h"
#include "CharacterController.h"
#include "BoxCollider.h"


namespace FlatEngine {
	RigidBody::RigidBody(long myID, long parentID)
	{
		SetType(ComponentTypes::RigidBody);
		SetID(myID);
		SetParentID(parentID);
		mass = 1;
		angularDrag = 1;
		gravity = 1;
		fallingGravity = gravity * 1.2f;		
		velocity = Vector2(0, 0);
		pendingForces = Vector2(0, 0);		
		acceleration = Vector2(0, 0);
		terminalVelocity = gravity * 0.4f;
		windResistance = 1.0f;  // Lower value = more resistance
		friction = 0.86f;  // 1 = no friction. 0 = velocity = 0
		equilibriumForce = 2;				
		_isGrounded = false;
		_isKinematic = false;
		_isStatic = false;
		forceCorrection = 0.03f;
	}

	RigidBody::RigidBody(std::shared_ptr<RigidBody> toCopy, long newParentID)
	{
		SetType(ComponentTypes::RigidBody);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		mass = toCopy->mass;
		angularDrag = toCopy->angularDrag;
		gravity = toCopy->gravity;
		fallingGravity = toCopy->fallingGravity;
		velocity = toCopy->velocity;
		pendingForces = Vector2(0, 0);
		acceleration = Vector2(0, 0);
		terminalVelocity = toCopy->terminalVelocity;
		windResistance = toCopy->windResistance;
		friction = toCopy->friction;
		equilibriumForce = toCopy->equilibriumForce;		
		_isGrounded = toCopy->_isGrounded;
		_isKinematic = toCopy->_isKinematic;
		_isStatic = toCopy->_isStatic;
		forceCorrection = toCopy->forceCorrection;
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
			{ "friction", friction },
			{ "equilibriumForce", equilibriumForce },
			{ "terminalVelocity", terminalVelocity },
			{ "windResistance", windResistance },
			{ "_isKinematic", _isKinematic },
			{ "_isStatic", _isStatic },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void RigidBody::CalculatePhysics()
	{
		// Add up forces
		ApplyCollisionForces();
		ApplyGravity();
		ApplyFriction();
		ApplyEquilibriumForce();
	

		// Apply them to RigidBody
		if (loadedProject->GetPhysicsSystem() == "Euler")
		{
			CalculateEulerPhysics();
		}
		else if (loadedProject->GetPhysicsSystem() == "Verlet")
		{
			// CalculateVerletPhysics();
		}
	}

	void RigidBody::CalculateEulerPhysics()
	{		
		if (mass == 0)
			acceleration = Vector2(pendingForces.x, pendingForces.y);
		else 
			acceleration = Vector2(pendingForces.x / mass, pendingForces.y / mass);
	}

	void RigidBody::CalculateVerletPhysics()
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
		velocity = Vector2(acceleration.x * deltaTime, acceleration.y * deltaTime);
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();

		transform->SetPosition(Vector2(position.x + velocity.x, position.y + velocity.y));
	}

	void RigidBody::ApplyVerletPhysics(float deltaTime)
	{
		// TODO
	}

	Vector2 RigidBody::AddVelocity(Vector2 vel)
	{
		// Make sure not colliding in that direction before adding the velocity
		std::shared_ptr<FlatEngine::BoxCollider> boxCollider = GetParent()->GetBoxCollider();
		if (boxCollider == nullptr || (vel.x > 0 && (!boxCollider->_isCollidingRight || !boxCollider->_rightCollisionStatic) || vel.x < 0 && (!boxCollider->_isCollidingLeft || !boxCollider->_leftCollisionStatic)))
			pendingForces.x += vel.x;

		pendingForces.y += vel.y;		
		return pendingForces;
	}

	void RigidBody::ApplyGravity()
	{
		if (gravity > 0)
		{
			if (!_isGrounded && velocity.y > -terminalVelocity)
			{
				if (velocity.y < 0)
					pendingForces.y -= fallingGravity;
				else
					pendingForces.y -= gravity;
			}
		}
		else if (gravity < 0)
		{
			if (!_isGrounded && velocity.y < terminalVelocity)
			{
				if (velocity.y > 0)
					pendingForces.y -= fallingGravity;
				else
					pendingForces.y -= gravity;
			}
		}
	}

	void RigidBody::ApplyFriction()
	{
		// Wind Friction
		if (gravity != 0 && !_isGrounded)
		{
			Vector2 dampenedVelocity = Vector2(pendingForces.x * windResistance, pendingForces.y * windResistance);
			pendingForces = dampenedVelocity;
		}

		// Get Character Controller for _isMoving
		std::shared_ptr<FlatEngine::CharacterController> characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
			characterController = GetParent()->GetCharacterController();

		bool _isMoving = false;
		if (characterController != nullptr)
			_isMoving = characterController->IsMoving();

		// Ground Friction
		if (!_isMoving && (gravity != 0 && _isGrounded))
		{
			Vector2 dampenedVelocity = Vector2(pendingForces.x * friction, pendingForces.y);
			pendingForces = dampenedVelocity;
		}	
		else if (gravity == 0)
		{
			Vector2 dampenedVelocity = Vector2(pendingForces.x * friction, pendingForces.y * friction);
			pendingForces = dampenedVelocity;
		}
	}

	void RigidBody::ApplyEquilibriumForce()
	{
		float maxSpeed = 1;
		std::shared_ptr<FlatEngine::CharacterController> characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
			characterController = GetParent()->GetCharacterController();

		if (characterController != nullptr)
			maxSpeed = characterController->GetMaxSpeed();

		if (characterController != nullptr || friction != 1)
		{
			// Horizontal speed control
			if (characterController != nullptr && velocity.x > maxSpeed || velocity.x > terminalVelocity)
				pendingForces.x -= equilibriumForce;
			else if (characterController != nullptr && velocity.x < -maxSpeed || velocity.x < -terminalVelocity)
				pendingForces.x += equilibriumForce;
			// Vertical speed control
			//if (velocity.y > maxSpeed)
			//	pendingForces.y -= equilibriumForce;
			//else if (velocity.y < -maxSpeed)
			//	pendingForces.y += equilibriumForce;		
		}
	}

	void RigidBody::ApplyCollisionForces()
	{
		std::vector<std::shared_ptr<FlatEngine::BoxCollider>> boxColliders = GetParent()->GetBoxColliders();						
		std::vector<std::shared_ptr<FlatEngine::CircleCollider>> circleColliders = GetParent()->GetCircleColliders();

		// BoxColliders
		for (std::shared_ptr<FlatEngine::BoxCollider> boxCollider : boxColliders)
		{
			Vector2 scale = Vector2(1, 1);
			std::shared_ptr<FlatEngine::Transform> transform = boxCollider->GetParent()->GetTransformComponent();
			if (transform != nullptr)
				scale = transform->GetScale();

			float halfWidth = boxCollider->GetActiveWidth() / 2 * scale.x;
			float halfHeight = boxCollider->GetActiveHeight() / 2 * scale.y;

			ApplyCollisionForce(boxCollider, halfWidth, halfHeight);
		}
		//CircleColliders
		for (std::shared_ptr<FlatEngine::CircleCollider> circleCollider : circleColliders)
		{
			float activeRadius = circleCollider->GetActiveRadiusGrid();			

			ApplyCollisionForce(circleCollider, activeRadius, activeRadius);
		}
	}

	void RigidBody::ApplyCollisionForce(std::shared_ptr<FlatEngine::Collider> collider, float halfWidth, float halfHeight)
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = GetParent()->GetTransformComponent()->GetPosition();				
		float newYPos;
		float newXPos;
		float cornerFriction = .01f;
		float cornerLerp = 0.3f;

		// Circle Colliders
		if (collider->GetTypeString() == "CircleCollider")
		{
			// Moving down
			if (pendingForces.y < 0)
			{
				if (collider->_isCollidingBottom && collider->_bottomCollisionSolid && (!collider->_isCollidingBottomLeft || (collider->_isCollidingBottomLeft && !collider->_bottomLeftCollisionSolid)) && (!collider->_isCollidingBottomRight || (collider->_isCollidingBottomRight && !collider->_bottomRightCollisionSolid)))
				{
					pendingForces.y = 0;
					newYPos = collider->bottomCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionSolid)
				{
					newXPos = collider->bottomLeftCollidedPosition.x;
					newYPos = collider->bottomLeftCollidedPosition.y;
					transform->SetPosition(Vector2(newXPos, newYPos));
				}
				else if (collider->_isCollidingBottomRight && collider->_bottomRightCollisionSolid)
				{
					newXPos = position.x - cornerFriction * -pendingForces.y; // Give some slide away from the edge based on pendingForces
					newYPos = collider->bottomRightCollidedPosition.y;
					//pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}	

			// Normal gravity
			if (gravity > 0 && (collider->_isCollidingBottom && collider->_bottomCollisionSolid) || (collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionSolid) || (collider->_isCollidingBottomRight && collider->_bottomRightCollisionSolid))
				_isGrounded = true;
			// Inverted gravity
			else if (gravity < 0 && (collider->_isCollidingTop && collider->_topCollisionSolid) || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid) || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid))
				_isGrounded = true;
			else
				_isGrounded = false;

			// Moving up
			if (pendingForces.y > 0)
			{
				if (collider->_isCollidingTop && collider->_topCollisionSolid && (!collider->_isCollidingTopLeft || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid)) && (!collider->_isCollidingTopRight || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid)))
				{
					// Inverted Gravity
					if (gravity < 0)
						_isGrounded = true;

					pendingForces.y = 0;
					newYPos = collider->topCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->_isCollidingTopLeft && collider->_topLeftCollisionSolid)
				{
					if (gravity < 0)
						_isGrounded = true;

					newXPos = position.x - cornerFriction * -pendingForces.y; // Give some slide away from the edge based on pendingForces
					newYPos = collider->topLeftCollidedPosition.y;
					pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopRight && collider->_topRightCollisionSolid)
				{
					if (gravity < 0)
						_isGrounded = true;

					newXPos = position.x + cornerFriction * -pendingForces.y; // Give some slide away from the edge based on pendingForces
					newYPos = collider->topRightCollidedPosition.y;
					pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (gravity < 0)
					_isGrounded = false;
			}
			// Moving left
			if (pendingForces.x < 0)
			{
				if (collider->_isCollidingLeft && collider->_leftCollisionSolid && (!collider->_isCollidingBottomLeft || (collider->_isCollidingBottomLeft && !collider->_bottomLeftCollisionSolid)) && (!collider->_isCollidingTopLeft || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid)))
				{
					pendingForces.x = 0;
					newXPos = collider->leftCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionSolid)
				{
					newYPos = position.y + cornerFriction * -pendingForces.x; // Give some slide away from the edge based on pendingForces
					newXPos = collider->bottomLeftCollidedPosition.x;
					pendingForces.x = 0;				
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopLeft && collider->_topLeftCollisionSolid)
				{
					newYPos = position.y - cornerFriction * -pendingForces.x; // Give some slide away from the edge based on pendingForces
					newXPos = collider->topLeftCollidedPosition.x;
					pendingForces.x = 0;					
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
			// Moving right
			if (pendingForces.x > 0)
			{
				if (collider->_isCollidingRight && collider->_rightCollisionSolid && (!collider->_isCollidingBottomRight || (collider->_isCollidingBottomRight && !collider->_bottomRightCollisionSolid)) && (!collider->_isCollidingTopRight || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid)))
				{
					pendingForces.x = 0;
					newXPos = collider->rightCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->_isCollidingBottomRight && collider->_bottomRightCollisionSolid)
				{
					newYPos = position.y + cornerFriction * pendingForces.x; // Give some slide away from the edge based on pendingForces
					newXPos = collider->bottomRightCollidedPosition.x;
					pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopRight && collider->_topRightCollisionSolid)
				{
					newYPos = position.y - cornerFriction * pendingForces.x; // Give some slide away from the edge based on pendingForces
					newXPos = collider->topRightCollidedPosition.x;
					pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
		}
		// Box Colliders
		else if (collider->GetTypeString() == "BoxCollider")
		{
			// "Floor" Collision Forces
			// 
			// Check if grounded normal gravity
			if (gravity != 0 && collider->_isCollidingBottom && collider->_bottomCollisionSolid || !collider->_isCollidingBottom && ((collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionStatic) || (collider->_isCollidingBottomRight && collider->_bottomRightCollisionStatic) && gravity > 0))
				//if ((collider->_isCollidingBottom || collider->_isCollidingBottomLeft || collider->_isCollidingBottomRight) && collider->_bottomCollisionStatic && collider->_bottomCollisionSolid && gravity > 0)
				_isGrounded = true;
			// Check if grounded inverted gravity
			else if (gravity != 0 && collider->_isCollidingBottom && collider->_bottomCollisionStatic && collider->_bottomCollisionSolid && gravity < 0)
				_isGrounded = true;
			else
				_isGrounded = false;

			if (_isGrounded)
			{
				// If grounded and normal gravity or inverted gravity
				if ((pendingForces.y < 0 && gravity > 0) || (pendingForces.y > 0 && gravity < 0))
				{
					pendingForces.y = 0;
				}

				if (collider->_isCollidingBottomRight)
				{
					newYPos = collider->bottomRightCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->_isCollidingBottomLeft)
				{
					newYPos = collider->bottomLeftCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->_isCollidingBottom)
				{
					newYPos = collider->bottomCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
			}

			// "Ceiling" Collision Forces
			//
			// Normal Gravity
			if (gravity > 0 && pendingForces.y > 0 && collider->_isCollidingTop && collider->_topCollisionSolid)
			{
				pendingForces.y = 0;
				transform->SetPosition(Vector2(position.x, collider->topCollidedPosition.y));
			}
			// Inverted Gravity
			if (gravity < 0 && pendingForces.y > 0 && collider->_isCollidingTop && collider->_topCollisionSolid)
			{
				pendingForces.y = 0;
				newYPos = collider->topCollision - halfHeight + 0.001f;
				transform->SetPosition(Vector2(position.x, newYPos));
			}

			// Horizontal Collision Forces
			// 
			if (collider->GetTypeString() == "BoxCollider")
			{
				// Collision on right side when moving to the right
				if (collider->_isCollidingRight && collider->_rightCollisionSolid && velocity.x > 0)
				{
					pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->rightCollidedPosition.x, position.y));
				}
				// Collision on left side when moving to the left
				else if (collider->_isCollidingLeft && collider->_leftCollisionSolid && velocity.x < 0)
				{
					pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->leftCollidedPosition.x, position.y));
				}
			}
			// If not BoxCollider and moving left up a corner while not colliding on bottom
			else if (velocity.x < 0 && collider->_isCollidingBottomLeft)
			{
				//transform->SetPosition(Vector2(position.x, newYPos));
				transform->SetPosition(collider->bottomLeftCollidedPosition);
			}
			// Collision on right side when moving to the right
			else if (collider->_isCollidingRight && collider->_rightCollisionStatic && collider->_rightCollisionSolid && velocity.x > 0)
			{
				pendingForces.x = 0;
				newXPos = collider->rightCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
			// Collision on left side when moving to the left
			else if (collider->_isCollidingLeft && collider->_leftCollisionStatic && collider->_leftCollisionSolid && velocity.x < 0)
			{
				pendingForces.x = 0;
				newXPos = collider->leftCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
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
		Vector2 nextVelocity = Vector2(acceleration.x * GetDeltaTime(), acceleration.y * GetDeltaTime());
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetTruePosition();
		return Vector2(position.x + nextVelocity.x * 2, position.y + nextVelocity.y * 2);
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

	void RigidBody::SetFallingGravity(float newFallingGravity)
	{
		fallingGravity = newFallingGravity;
	}

	float RigidBody::GetFallingGravity()
	{
		return fallingGravity;
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

	void RigidBody::SetEquilibriumForce(float newEquilibriumForce)
	{
		equilibriumForce = newEquilibriumForce;
	}

	Vector2 RigidBody::GetVelocity()
	{
		return velocity;
	}

	Vector2 RigidBody::GetAcceleration()
	{
		return acceleration;
	}

	Vector2 RigidBody::GetPendingForces()
	{
		return pendingForces;
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
	
	void RigidBody::SetPendingForces(Vector2 newPendingForces)
	{
		pendingForces = newPendingForces;		
	}

	void RigidBody::SetWindResistance(float newWindResistance)
	{
		windResistance = newWindResistance;
	}

	float RigidBody::GetWindResistance()
	{
		return windResistance;
	}

	float RigidBody::GetFriction()
	{
		return friction;
	}

	void RigidBody::SetFriction(float newFriction)
	{
		friction = newFriction;
	}

	float RigidBody::GetEquilibriumForce()
	{
		return equilibriumForce;
	}
}