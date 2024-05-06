#pragma once
#include "Component.h"
#include "Vector2.h"
#include <string>

namespace FlatEngine {
	class CharacterController : public Component
	{
	public:
		CharacterController(long myID, long parentID);
		CharacterController(std::shared_ptr<CharacterController> toCopy, long newParentID);
		~CharacterController();

		std::string GetData();
		void MoveToward(Vector2 direction);
		void MoveTo(Vector2 location);
		void SetWalkSpeed(float speed);
		float GetWalkSpeed();
		void SetRunSpeed(float speed);
		float GetRunSpeed();
		void SetGravity(float gravity);
		float GetGravity();
		void SetMoving(bool _isMoving);
		bool IsMoving();
		void SetVelocity(float velocity);
		float GetVelocity();
		void SetIsGrounded(bool _grounded);
		bool IsGrounded();

	private:
		float walkSpeed;
		float runSpeed;
		float maxSpeed;
		float speedCorrection;
		float gravity;
		bool _isMoving;
		float velocity;
		bool _isGrounded;

	};
}


