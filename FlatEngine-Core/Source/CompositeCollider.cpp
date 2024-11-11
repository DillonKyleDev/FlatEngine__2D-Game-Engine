#include "CompositeCollider.h"


namespace FlatEngine
{
	CompositeCollider::CompositeCollider(long myID, long parentID)
	{
		SetType(T_CompositeCollider);
		SetID(myID);
		SetParentID(parentID);
		m_colliders = std::vector<std::shared_ptr<Collider>>();
	}

	CompositeCollider::~CompositeCollider()
	{
	}
}