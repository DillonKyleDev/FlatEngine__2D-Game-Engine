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
		

	private:
		float moveSpeed;
		bool _isMoving;
		float velocity;
	};
}


