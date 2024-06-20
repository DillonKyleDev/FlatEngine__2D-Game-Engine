#pragma once
#include "Component.h"
#include "Collider.h"


namespace FlatEngine
{
	class CompositeCollider : public Collider
	{
	public:
		CompositeCollider(long myID = -1, long parentID = -1);
		CompositeCollider(std::shared_ptr<Collider> toCopy, long newParentID);
		~CompositeCollider();

	private:
		std::vector<std::shared_ptr<Collider>> colliders;	
	};
}