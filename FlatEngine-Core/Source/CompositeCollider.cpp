#include "CompositeCollider.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	CompositeCollider::CompositeCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::T_CompositeCollider);
		SetID(myID);
		SetParentID(parentID);
		m_colliders = std::vector<std::shared_ptr<Collider>>();
	}

	CompositeCollider::~CompositeCollider()
	{
	}
}