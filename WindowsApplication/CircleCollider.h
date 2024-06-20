#pragma once
#include "Collider.h"
#include "FlatEngine.h"

namespace FlatEngine 
{
	class CircleCollider : public Collider
	{
		friend class Collider;
		friend class BoxCollider;
		class GameObject;

	public:
		CircleCollider(long myID = -1, long parentID = -1);
		CircleCollider(std::shared_ptr<CircleCollider> toCopy, long newParentID);
		~CircleCollider();

		void UpdateCenter();
		void UpdateActiveEdges();
		void SharedAxisUpdateActiveEdges();
		std::string GetData();
		void RecalculateBounds();	
		//void ResetCollisions();

	private:
		bool _activeEdgesSet;

		// Current
		float activeLeft;
		float activeRight;
		float activeBottom;
		float activeTop;

		// Next
		float nextActiveLeft;
		float nextActiveRight;
		float nextActiveBottom;
		float nextActiveTop;
	};
}