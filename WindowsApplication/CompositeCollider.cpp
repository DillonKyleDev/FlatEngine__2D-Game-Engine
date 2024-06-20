#include "CompositeCollider.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	CompositeCollider::CompositeCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::CompositeCollider);
		SetID(myID);
		SetParentID(parentID);
	}
	CompositeCollider::CompositeCollider(std::shared_ptr<Collider> toCopy, long newParentID)
	{
		SetType(ComponentTypes::CompositeCollider);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
	}
	CompositeCollider::~CompositeCollider()
	{
	}
}