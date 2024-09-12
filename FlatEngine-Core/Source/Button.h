#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Vector4.h"

#include <string>

namespace FlatEngine
{
	class GameObject;

	class Button : public Component
	{
	public:
		Button(long myID = -1, long parentID = -1);
		~Button();
		std::string GetData();

		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
		void SetMouseIsOver(bool b_isOver);
		void SetIsOverFired(bool b_fired);
		bool MouseIsOver();
		//ImVec4(activeTop, activeRight, activeBottom, activeLeft)
		void CalculateActiveEdges();
		Vector4 GetActiveEdges();

	private:
		bool m_b_mouseIsOver;
		bool m_b_hasMouseOverFired;
		float m_activeWidth;
		float m_activeHeight;
		Vector4 m_activeEdges;
		Vector2 m_activeOffset;
		int m_activeLayer;
	};
}