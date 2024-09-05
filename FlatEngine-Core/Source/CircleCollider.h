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
		~CircleCollider();

		void UpdateCenter(float step, Vector2 centerPoint);
		void UpdateActiveEdges(float step, Vector2 centerPoint);
		std::string GetData();
		void RecalculateBounds(float step, Vector2 centerPoint);

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