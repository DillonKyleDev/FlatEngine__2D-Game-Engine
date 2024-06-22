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
		void SetMaxAcceleration(float speed);
		float GetMaxAcceleration();
		void SetMaxSpeed(float newMaxSpeed);
		float GetMaxSpeed();
		void SetAirControl(float newAirControl);
		float GetAirControl();
		void SetMoving(bool _isMoving);
		bool IsMoving();

	private:
		float maxAcceleration;
		float maxSpeed;
		float airControl;
		float speedCorrection;
		bool _isMoving;
	};
}


