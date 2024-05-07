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
		void CalculatePhysics(float deltaTime);
		void ApplyPhysics();
		Vector2 AddVelocity(Vector2 vel, float deltaTime);
		void ApplyGravity(float deltaTime);
		void ApplyVelocity();
		void AddForce(Vector2 direction, float power);
		void Move(Vector2 position);
		void SetMass(float newMass);
		float GetMass();
		void SetAngularDrag(float newAngularDrag);
		float GetAngularDrag();
		void SetGravity(float newGravity);
		float GetGravity();
		void SetVelocity(Vector2 newVelocity);
		Vector2 GetVelocity();
		Vector2 GetPendingVelocity();
		Vector2 GetNextPosition();
		void SetTerminalVelocity(float newTerminalVelocity);
		float GetTerminalVelocity();
		void ApplyFriction(float deltaTime);
		void SetIsMoving(bool _moving);
		bool IsMoving();
		void SetIsContinuous(bool _continuous);
		bool IsContinuous();
		bool IsStatic();
		void SetIsStatic(bool _static);
		void SetIsGrounded(bool _grounded);
		bool IsGrounded();
		void SetIsKinematic(bool _kinematic);
		bool IsKinematic();

	private:
		float mass;
		float angularDrag;
		float gravity;
		float gravityCorrection;
		Vector2 velocity;
		Vector2 pendingVelocity;
		bool _isMoving;
		bool _isContinious;
		bool _isGrounded;
		bool _isKinematic;
		bool _isStatic;
		float terminalVelocity;
		float windResistance;
		float friction;
		float forceCorrection;
	};
}