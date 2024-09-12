#pragma once
#include "Collider.h"


namespace FlatEngine 
{
	class CircleCollider : public Collider
	{
		friend class Collider;
		friend class BoxCollider;

	public:
		CircleCollider(long myID = -1, long parentID = -1);
		~CircleCollider();
		std::string GetData();

		void UpdateCenter(float step, Vector2 centerPoint);
		void UpdateActiveEdges(float step, Vector2 centerPoint);
		void RecalculateBounds(float step, Vector2 centerPoint);

	private:
		bool m_b_activeEdgesSet;
		// Current
		float m_activeLeft;
		float m_activeRight;
		float m_activeBottom;
		float m_activeTop;
		// Next
		float m_nextActiveLeft;
		float m_nextActiveRight;
		float m_nextActiveBottom;
		float m_nextActiveTop;
	};
}