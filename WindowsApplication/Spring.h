#pragma once
#include "FlatEngine.h"
#include "Component.h"

namespace FlatEngine
{
	class GameObject;

	class Spring : public Component
	{
	public:
		Spring();
		Spring(const Spring* toCopy);
		~Spring();

		std::shared_ptr<GameObject> attachedTo;
		float radius;
	private:


	};
}