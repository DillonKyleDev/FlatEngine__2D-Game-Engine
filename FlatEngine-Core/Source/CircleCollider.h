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
		CircleCollider(CircleCollider* toCopy, long newParentID, long myID = -1);
		~CircleCollider();

		void UpdateCenter(float step, Vector2 centerPoint);
		void UpdateActiveEdges(std::string collisionDetectionType, float step, Vector2 centerPoint);
		void SharedAxisUpdateActiveEdges(float step, Vector2 centerPoint);
		std::string GetData();
		void RecalculateBounds(std::string collisionDetectionType, float step, Vector2 centerPoint);
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