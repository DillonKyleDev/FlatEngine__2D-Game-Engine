#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Texture.h"
#include "Vector2.h"
#include "GameObject.h"
#include <functional>




namespace FlatEngine
{
	class Button : public Component
	{
	public:
		Button(long myID = -1, long parentID = -1);
		~Button();
		
		void OnMouseOver(std::function<void(std::shared_ptr<GameObject>)> callback);
		void OnMouseEnter(std::function<void(std::shared_ptr<GameObject>)> callback);
		void OnMouseLeave(std::function<void(std::shared_ptr<GameObject>)> callback);
		void OnMouseLeftClick(std::function<void(std::shared_ptr<GameObject>)> callback);
		void OnMouseRightClick(std::function<void(std::shared_ptr<GameObject>)> callback);
		void SetActive(bool _active);
		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		bool IsActive();
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
		void SetMouseIsOver(bool _isOver);
		void SetIsOverFired(bool _fired);
		bool MouseIsOver();
		//ImVec4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(ImVec4 edges);
		ImVec4 GetActiveEdges();
		std::string GetData();

		std::function<void(std::shared_ptr<GameObject>)> OnMouseOverFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnMouseEnterFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnMouseLeaveFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnLeftClickFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnRightClickFunction;

		bool MouseOverSet();
		bool MouseEnterSet();
		bool MouseLeaveSet();
		bool LeftClickSet();
		bool RightClickSet();

		bool _mouseOverSet;
		bool _mouseEnterSet;
		bool _mouseLeaveSet;
		bool _leftClickSet;
		bool _rightClickSet;

	private:
		bool _mouseIsOver;
		bool _hasMouseOverFired;
		bool _active;
		float activeWidth;
		float activeHeight;
		ImVec4 activeEdges;
		Vector2 activeOffset;
		int activeLayer;
	};
}