#pragma once
#include "FlatEngine.h"
#include "Component.h"
#include "Vector2.h"
#include <string>


namespace FlatEngine 
{

	class RigidBody : public Component
	{
	public:
		RigidBody(long myID = -1, long parentID = -1);
		RigidBody(std::shared_ptr<RigidBody> toCopy, long newParentID);
		~RigidBody();

		std::string GetData();
		void ApplyGravity();
		void AddForce(Vector2 direction, float power);
		void Move(Vector2 position);
		void SetMass(float newMass);
		float GetMass();
		void SetAngularDrag(float newAngularDrag);
		float GetAngularDrag();
		void SetGravity(float newGravity);
		float GetGravity();
		void SetVelocity(float newVelocity);
		float GetVelocity();
		void SetIsMoving(bool _moving);
		bool IsMoving();
		void SetIsContinuous(bool _continuous);
		bool IsContinuous();
		void SetIsGrounded(bool _grounded);
		bool IsGrounded();
		void SetIsKinematic(bool _kinematic);
		bool IsKinematic();

	private:
		float mass;
		float angularDrag;
		float gravity;
		float velocity;
		bool _isMoving;
		bool _isContinious;
		bool _isGrounded;
		bool _isKinematic;
	};
}