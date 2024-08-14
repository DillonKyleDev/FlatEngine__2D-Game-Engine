#include "CompositeCollider.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	CompositeCollider::CompositeCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::T_CompositeCollider);
		SetID(myID);
		SetParentID(parentID);
	}
	CompositeCollider::CompositeCollider(std::shared_ptr<Collider> toCopy, long newParentID, long myID)
	{
		SetType(ComponentTypes::T_CompositeCollider);
		if (myID != -1)
			SetID(myID);
		else
			SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
	}
	CompositeCollider::~CompositeCollider()
	{
	}
}