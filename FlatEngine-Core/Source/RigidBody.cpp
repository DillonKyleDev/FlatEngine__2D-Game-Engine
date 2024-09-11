#include "RigidBody.h"
#include "Transform.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "Project.h"
#include "Sprite.h"


namespace FlatEngine {
	RigidBody::RigidBody(long myID, long parentID)
	{
		SetType(T_RigidBody);
		SetID(myID);
		SetParentID(parentID);
		// Linear
		m_mass = 1;
		m_1overMass = 1 / m_mass;		
		m_pendingForces = Vector2(0, 0);
		m_velocity = Vector2(0, 0);	
		m_acceleration = Vector2(0, 0);
		m_friction = 0.86f;  // 1 = no m_friction. 0 = m_velocity = 0
		// Rotational
		m_I = 1; // Moment of inertia = 2mass / 5 * (r^2) (based on sprite dimensions)
		m_1overI = 1;
		m_pendingTorques = 0;
		m_angularVelocity = 0;
		m_angularAcceleration = 0;
		m_angularDrag = 1;
		m_b_allowTorques = true;

		m_equilibriumForce = 2;				
		m_b_isGrounded = false;
		m_b_isStatic = false;
		m_forceCorrection = 0.01f;
		m_gravity = 1;
		m_fallingGravity = m_gravity * 1.2f;
		m_terminalVelocity = m_gravity * 0.4f;
		m_windResistance = 1.0f;  // Lower value = more resistance
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
			{ "mass", m_mass},
			{ "angularDrag", m_angularDrag },
			{ "gravity", m_gravity },
			{ "fallingGravity", m_fallingGravity },
			{ "friction", m_friction },
			{ "equilibriumForce", m_equilibriumForce },
			{ "terminalVelocity", m_terminalVelocity },
			{ "windResistance", m_windResistance },
			{ "_isStatic", m_b_isStatic },
			{ "_allowTorques", m_b_allowTorques },
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
	
		// Linear
		if (m_mass == 0)
			m_acceleration = Vector2(m_pendingForces.x, m_pendingForces.y);
		else
			m_acceleration = Vector2(m_pendingForces.x * m_1overMass * m_forceCorrection, m_pendingForces.y * m_1overMass * m_forceCorrection);

		// Rotational - Torque = I * angularAcceleration
		if (m_I == 0)
			m_angularAcceleration = m_pendingTorques;
		else
			m_angularAcceleration = m_pendingTorques * m_1overI;
	}

	void RigidBody::ApplyPhysics(float deltaTime)
	{
		// In reality, we should be adding acceleration to velocity, but it is difficult to simulate opposing forces that would actually cause objects to slow down (ie. give them negative values in the opposing direction at all times)
		m_velocity = Vector2(m_acceleration.x, m_acceleration.y);
		FlatEngine::Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();

		transform->SetPosition(Vector2(position.x + m_velocity.x, position.y + m_velocity.y));

		if (m_b_allowTorques)
			m_angularVelocity = m_angularAcceleration;

		float rotation = transform->GetRotation();
		transform->SetRotation((float)fmod(rotation + m_angularVelocity, 360));
	}

	Vector2 RigidBody::AddVelocity(Vector2 vel)
	{
		// Make sure not colliding in that direction before adding the m_velocity
		std::vector<FlatEngine::BoxCollider*> boxColliders = GetParent()->GetBoxColliders();
		for (FlatEngine::BoxCollider* boxCollider : boxColliders)
		{
			if ((vel.x > 0 && (!boxCollider->_isCollidingRight || !boxCollider->_rightCollisionStatic) || vel.x < 0 && (!boxCollider->_isCollidingLeft || !boxCollider->_leftCollisionStatic)))
				m_pendingForces.x += vel.x;
		}

		m_pendingForces.y += vel.y;		
		return m_pendingForces;
	}

	void RigidBody::ApplyGravity()
	{
		if (m_gravity > 0)
		{
			if (!m_b_isGrounded && m_velocity.y > -m_terminalVelocity)
			{
				if (m_velocity.y < 0)
					m_pendingForces.y -= m_fallingGravity;
				else
					m_pendingForces.y -= m_gravity;
			}
		}
		else if (m_gravity < 0)
		{
			if (!m_b_isGrounded && m_velocity.y < m_terminalVelocity)
			{
				if (m_velocity.y > 0)
					m_pendingForces.y -= m_fallingGravity;
				else
					m_pendingForces.y -= m_gravity;
			}
		}
	}

	void RigidBody::ApplyFriction()
	{
		// Wind resistance
		if (m_gravity != 0 && !m_b_isGrounded)
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_windResistance, m_pendingForces.y * m_windResistance);
			m_pendingForces = dampenedVelocity;
		}

		// Rotational drag
		if (m_b_allowTorques)
			m_pendingTorques *= m_angularDrag;

		// Get Character Controller for _isMoving
		FlatEngine::CharacterController* characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
			characterController = GetParent()->GetCharacterController();

		bool _isMoving = false;
		if (characterController != nullptr)
			_isMoving = characterController->IsMoving();

		// Ground m_friction
		if (!_isMoving && (m_gravity != 0 && m_b_isGrounded))
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_friction, m_pendingForces.y);
			m_pendingForces = dampenedVelocity;
		}	
		else if (m_gravity == 0)
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_friction, m_pendingForces.y * m_friction);
			m_pendingForces = dampenedVelocity;
		}
	}

	void RigidBody::ApplyEquilibriumForce()
	{
		float maxSpeed = 1;
		FlatEngine::CharacterController* characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
			characterController = GetParent()->GetCharacterController();

		if (characterController != nullptr)
			maxSpeed = characterController->GetMaxSpeed();

		if (characterController != nullptr || m_friction != 1)
		{
			// Horizontal speed control
			if (characterController != nullptr && m_velocity.x > maxSpeed || m_velocity.x > m_terminalVelocity)
				m_pendingForces.x -= m_equilibriumForce;
			else if (characterController != nullptr && m_velocity.x < -maxSpeed || m_velocity.x < -m_terminalVelocity)
				m_pendingForces.x += m_equilibriumForce;
			// Vertical speed control
			//if (m_velocity.y > maxSpeed)
			//	m_pendingForces.y -= m_equilibriumForce;
			//else if (m_velocity.y < -maxSpeed)
			//	m_pendingForces.y += m_equilibriumForce;		
		}
	}

	void RigidBody::ApplyCollisionForces()
	{
		std::vector<FlatEngine::BoxCollider*> boxColliders = GetParent()->GetBoxColliders();
		std::vector<FlatEngine::CircleCollider*> circleColliders = GetParent()->GetCircleColliders();

		// BoxColliders
		for (FlatEngine::BoxCollider* boxCollider : boxColliders)
		{
			Vector2 scale = Vector2(1, 1);
			FlatEngine::Transform* transform = boxCollider->GetParent()->GetTransform();
			if (transform != nullptr)
				scale = transform->GetScale();

			float halfWidth = boxCollider->GetActiveWidth() / 2 * scale.x;
			float halfHeight = boxCollider->GetActiveHeight() / 2 * scale.y;

			ApplyCollisionForce(boxCollider, halfWidth, halfHeight);
		}
		//CircleColliders
		for (FlatEngine::CircleCollider* circleCollider : circleColliders)
		{
			float activeRadius = circleCollider->GetActiveRadiusGrid();			

			ApplyCollisionForce(circleCollider, activeRadius, activeRadius);
		}
	}

	void RigidBody::ApplyCollisionForce(FlatEngine::Collider* collider, float halfWidth, float halfHeight)
	{
		FlatEngine::Transform* transform = GetParent()->GetTransform();
		Vector2 position = GetParent()->GetTransform()->GetPosition();				
		float newYPos;
		float newXPos;
		float cornerm_friction = .01f;
		float cornerLerp = 0.3f;

		// Circle Colliders
		if (collider->GetTypeString() == "CircleCollider")
		{
			// Moving down
			if (m_pendingForces.y < 0)
			{
				if (collider->_isCollidingBottom && collider->_bottomCollisionSolid && (!collider->_isCollidingBottomLeft || (collider->_isCollidingBottomLeft && !collider->_bottomLeftCollisionSolid)) && (!collider->_isCollidingBottomRight || (collider->_isCollidingBottomRight && !collider->_bottomRightCollisionSolid)))
				{
					m_pendingForces.y = 0;
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
					newXPos = position.x - cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->bottomRightCollidedPosition.y;
					//m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}	

			// Normal m_gravity
			if (m_gravity > 0 && (collider->_isCollidingBottom && collider->_bottomCollisionSolid) || (collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionSolid) || (collider->_isCollidingBottomRight && collider->_bottomRightCollisionSolid))
				m_b_isGrounded = true;
			// Inverted m_gravity
			else if (m_gravity < 0 && (collider->_isCollidingTop && collider->_topCollisionSolid) || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid) || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid))
				m_b_isGrounded = true;
			else
				m_b_isGrounded = false;

			// Moving up
			if (m_pendingForces.y > 0)
			{
				if (collider->_isCollidingTop && collider->_topCollisionSolid && (!collider->_isCollidingTopLeft || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid)) && (!collider->_isCollidingTopRight || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid)))
				{
					// Inverted m_gravity
					if (m_gravity < 0)
						m_b_isGrounded = true;

					m_pendingForces.y = 0;
					newYPos = collider->topCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->_isCollidingTopLeft && collider->_topLeftCollisionSolid)
				{
					if (m_gravity < 0)
						m_b_isGrounded = true;

					newXPos = position.x - cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->topLeftCollidedPosition.y;
					m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopRight && collider->_topRightCollisionSolid)
				{
					if (m_gravity < 0)
						m_b_isGrounded = true;

					newXPos = position.x + cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->topRightCollidedPosition.y;
					m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (m_gravity < 0)
					m_b_isGrounded = false;
			}
			// Moving left
			if (m_pendingForces.x < 0)
			{
				if (collider->_isCollidingLeft && collider->_leftCollisionSolid && (!collider->_isCollidingBottomLeft || (collider->_isCollidingBottomLeft && !collider->_bottomLeftCollisionSolid)) && (!collider->_isCollidingTopLeft || (collider->_isCollidingTopLeft && !collider->_topLeftCollisionSolid)))
				{
					m_pendingForces.x = 0;
					newXPos = collider->leftCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionSolid)
				{
					newYPos = position.y + cornerm_friction * -m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->bottomLeftCollidedPosition.x;
					m_pendingForces.x = 0;				
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopLeft && collider->_topLeftCollisionSolid)
				{
					newYPos = position.y - cornerm_friction * -m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->topLeftCollidedPosition.x;
					m_pendingForces.x = 0;					
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
			// Moving right
			if (m_pendingForces.x > 0)
			{
				if (collider->_isCollidingRight && collider->_rightCollisionSolid && (!collider->_isCollidingBottomRight || (collider->_isCollidingBottomRight && !collider->_bottomRightCollisionSolid)) && (!collider->_isCollidingTopRight || (collider->_isCollidingTopRight && !collider->_topRightCollisionSolid)))
				{
					m_pendingForces.x = 0;
					newXPos = collider->rightCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->_isCollidingBottomRight && collider->_bottomRightCollisionSolid)
				{
					newYPos = position.y + cornerm_friction * m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->bottomRightCollidedPosition.x;
					m_pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->_isCollidingTopRight && collider->_topRightCollisionSolid)
				{
					newYPos = position.y - cornerm_friction * m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->topRightCollidedPosition.x;
					m_pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
		}
		// Box Colliders
		else if (collider->GetTypeString() == "BoxCollider")
		{
			// "Floor" Collision Forces
			// 
			// Check if grounded normal m_gravity
			if (m_gravity != 0 && collider->_isCollidingBottom && collider->_bottomCollisionSolid || !collider->_isCollidingBottom && ((collider->_isCollidingBottomLeft && collider->_bottomLeftCollisionStatic) || (collider->_isCollidingBottomRight && collider->_bottomRightCollisionStatic) && m_gravity > 0))
				//if ((collider->_isCollidingBottom || collider->_isCollidingBottomLeft || collider->_isCollidingBottomRight) && collider->_bottomCollisionStatic && collider->_bottomCollisionSolid && m_gravity > 0)
				m_b_isGrounded = true;
			// Check if grounded inverted m_gravity
			else if (m_gravity != 0 && collider->_isCollidingBottom && collider->_bottomCollisionStatic && collider->_bottomCollisionSolid && m_gravity < 0)
				m_b_isGrounded = true;
			else
				m_b_isGrounded = false;

			if (m_b_isGrounded)
			{
				// If grounded and normal m_gravity or inverted m_gravity
				if ((m_pendingForces.y < 0 && m_gravity > 0) || (m_pendingForces.y > 0 && m_gravity < 0))
				{
					m_pendingForces.y = 0;
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
			// Normal m_gravity
			if (m_gravity > 0 && m_pendingForces.y > 0 && collider->_isCollidingTop && collider->_topCollisionSolid)
			{
				m_pendingForces.y = 0;
				transform->SetPosition(Vector2(position.x, collider->topCollidedPosition.y));
			}
			// Inverted m_gravity
			if (m_gravity < 0 && m_pendingForces.y > 0 && collider->_isCollidingTop && collider->_topCollisionSolid)
			{
				m_pendingForces.y = 0;
				newYPos = collider->topCollision - halfHeight + 0.001f;
				transform->SetPosition(Vector2(position.x, newYPos));
			}

			// Horizontal Collision Forces
			// 
			if (collider->GetTypeString() == "BoxCollider")
			{
				// Collision on right side when moving to the right
				if (collider->_isCollidingRight && collider->_rightCollisionSolid && m_velocity.x > 0)
				{
					m_pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->rightCollidedPosition.x, position.y));
				}
				// Collision on left side when moving to the left
				else if (collider->_isCollidingLeft && collider->_leftCollisionSolid && m_velocity.x < 0)
				{
					m_pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->leftCollidedPosition.x, position.y));
				}
			}
			// If not BoxCollider and moving left up a corner while not colliding on bottom
			else if (m_velocity.x < 0 && collider->_isCollidingBottomLeft)
			{
				//transform->SetPosition(Vector2(position.x, newYPos));
				transform->SetPosition(collider->bottomLeftCollidedPosition);
			}
			// Collision on right side when moving to the right
			else if (collider->_isCollidingRight && collider->_rightCollisionStatic && collider->_rightCollisionSolid && m_velocity.x > 0)
			{
				m_pendingForces.x = 0;
				newXPos = collider->rightCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
			// Collision on left side when moving to the left
			else if (collider->_isCollidingLeft && collider->_leftCollisionStatic && collider->_leftCollisionSolid && m_velocity.x < 0)
			{
				m_pendingForces.x = 0;
				newXPos = collider->leftCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
		}
	}

	void RigidBody::AddForce(Vector2 force, float multiplier)
	{
		// Normalize the force first, then apply the power factor to the force
		Vector2 addedForce = Vector2(force.x * multiplier, force.y * multiplier);
		m_pendingForces.x += addedForce.x;
		m_pendingForces.y += addedForce.y;		
	}

	void RigidBody::AddTorque(float torque, float multiplier)
	{
		if (m_b_allowTorques)
		{
			float addedTorque = torque * multiplier;
			m_pendingTorques += addedTorque;
		}
	}

	Vector2 RigidBody::GetNextPosition()
	{
		Vector2 nextVelocity = Vector2(m_acceleration.x, m_acceleration.y);
		FlatEngine::Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetTruePosition();
		return Vector2(position.x + nextVelocity.x * 2, position.y + nextVelocity.y * 2);
	}

	void RigidBody::SetMass(float mass)
	{
		m_mass = mass;
		m_1overMass = 1 / m_mass;
		m_I = 2* m_mass / 5 * 9;

		// Get new moment of inertia
		if (GetParent() != nullptr)
		{
			Sprite* sprite = GetParent()->GetSprite();
			if (sprite != nullptr)
			{
				float width = sprite->GetTextureWidth() / 10;
				float height = sprite->GetTextureHeight() / 10;
				float radius = (width + height) / 2; // rough estimate
				if (width != 0 && height != 0)
					m_I = 2 * m_mass / 5 * radius * radius;
			}
		}

		m_1overI = 1 / m_I;
	}

	float RigidBody::GetMass()
	{
		return m_mass;
	}

	float RigidBody::GetI()
	{
		return m_I;
	}

	void RigidBody::UpdateI()
	{
		// Get new moment of inertia
		if (GetParent() != nullptr)
		{
			Sprite* sprite = GetParent()->GetSprite();
			if (sprite != nullptr)
			{
				float width = sprite->GetTextureWidth() / 10;
				float height = sprite->GetTextureHeight() / 10;
				float radius = (width + height) / 2; // rough estimate
				if (width != 0 && height != 0)
					m_I = 2 * m_mass / 5 * radius * radius;
			}
		}

		m_1overI = 1 / m_I;
	}

	void RigidBody::SetTorquesAllowed(bool b_allowed)
	{
		m_b_allowTorques = b_allowed;
	}

	bool RigidBody::TorquesAllowed()
	{
		return m_b_allowTorques;
	}

	void RigidBody::SetAngularDrag(float angularDrag)
	{
		m_angularDrag = angularDrag;
	}

	float RigidBody::GetAngularDrag()
	{
		return m_angularDrag;
	}

	void RigidBody::SetGravity(float gravity)
	{
		m_gravity = gravity;
	}

	float RigidBody::GetGravity()
	{
		return m_gravity;
	}

	void RigidBody::SetFallingGravity(float fallingGravity)
	{
		m_fallingGravity = fallingGravity;
	}

	float RigidBody::GetFallingGravity()
	{
		return m_fallingGravity;
	}

	void RigidBody::SetVelocity(Vector2 velocity)
	{
		m_velocity = velocity;
	}

	void RigidBody::SetTerminalVelocity(float terminalVelocity)
	{
		m_terminalVelocity = terminalVelocity;
	}

	float RigidBody::GetTerminalVelocity()
	{
		return m_terminalVelocity;
	}

	void RigidBody::SetEquilibriumForce(float equilibriumForce)
	{
		m_equilibriumForce = equilibriumForce;
	}

	Vector2 RigidBody::GetVelocity()
	{
		return m_velocity;
	}

	Vector2 RigidBody::GetAcceleration()
	{
		return m_acceleration;
	}

	Vector2 RigidBody::GetPendingForces()
	{
		return m_pendingForces;
	}

	float RigidBody::GetAngularVelocity()
	{
		return m_angularVelocity;
	}

	float RigidBody::GetAngularAcceleration()
	{
		return m_angularAcceleration;
	}

	float RigidBody::GetPendingTorques()
	{
		return m_pendingTorques;
	}

	void RigidBody::SetIsStatic(bool b_static)
	{
		m_b_isStatic = b_static;
	}

	bool RigidBody::IsStatic()
	{
		return m_b_isStatic;
	}

	void RigidBody::SetIsGrounded(bool b_grounded)
	{
		m_b_isGrounded = b_grounded;
	}

	bool RigidBody::IsGrounded()
	{
		return m_b_isGrounded;
	}
	
	void RigidBody::SetPendingForces(Vector2 pendingForces)
	{
		m_pendingForces = pendingForces;
	}

	void RigidBody::SetAngularVelocity(float angularVelocity)
	{
		m_angularVelocity = angularVelocity;
	}

	void RigidBody::SetWindResistance(float windResistance)
	{
		m_windResistance = windResistance;
	}

	float RigidBody::GetWindResistance()
	{
		return m_windResistance;
	}

	float RigidBody::GetFriction()
	{
		return m_friction;
	}

	void RigidBody::SetFriction(float friction)
	{
		m_friction = friction;
	}

	float RigidBody::GetEquilibriumForce()
	{
		return m_equilibriumForce;
	}
}