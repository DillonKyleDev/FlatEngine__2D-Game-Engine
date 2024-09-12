#pragma once
#include "Component.h"
#include "Collider.h"

#include <vector>
#include <memory>


namespace FlatEngine
{
	class CompositeCollider : public Collider
	{
	public:
		CompositeCollider(long myID = -1, long parentID = -1);
		~CompositeCollider();

	private:
		std::vector<std::shared_ptr<Collider>> m_colliders;	
	};
}