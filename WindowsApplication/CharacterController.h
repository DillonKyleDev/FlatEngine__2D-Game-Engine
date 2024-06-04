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
		void SetMaxAcceleration(float speed);
		float GetMaxAcceleration();
		void SetMaxSpeed(float newMaxSpeed);
		float GetMaxSpeed();
		void SetAirControl(float newAirControl);
		float GetAirControl();
		void SetMoving(bool _isMoving);
		bool IsMoving();
		void SetIsGrounded(bool _grounded);
		bool IsGrounded();

	private:
		float maxAcceleration;
		float maxSpeed;
		float airControl;
		float speedCorrection;
		bool _isMoving;
		bool _isGrounded;
	};
}


