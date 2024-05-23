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

		void CalculatePhysics();
		void CalculateEulerPhysics();
		void CalculateVerletPhysics();

		void ApplyPhysics(float deltaTime);
		void ApplyEulerPhysics(float deltaTime);
		void ApplyVerletPhysics(float deltaTime);

		Vector2 AddVelocity(Vector2 vel);
		void ApplyGravity();
		void AddForce(Vector2 direction, float power);
		void ApplyEquilibriumForce();

		void Move(Vector2 position);
		void SetMass(float newMass);
		float GetMass();
		void SetAngularDrag(float newAngularDrag);
		float GetAngularDrag();
		void SetGravity(float newGravity);
		float GetGravity();
		void SetVelocity(Vector2 newVelocity);
		Vector2 GetVelocity();
		Vector2 GetPendingForces();
		void SetPendingForces(Vector2 newPendingForces);
		Vector2 GetNextPosition();
		void SetTerminalVelocity(float newTerminalVelocity);
		float GetTerminalVelocity();
		float GetEquilibriumForce();
		void ApplyFriction();
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
		float fallingGravity;
		float gravityCorrection;
		Vector2 velocity;
		Vector2 pendingForces;
		Vector2 acceleration;
		bool _isMoving;
		bool _isContinious;
		bool _isGrounded;
		bool _isKinematic;
		bool _isStatic;
		float terminalVelocity;
		float windResistance;
		float friction;
		float equilibriumForce;
		float forceCorrection;
	};
}