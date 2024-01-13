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
		Button(long myID, long parentID);
		~Button();
		
		void OnMouseOver(std::function<void(std::shared_ptr<GameObject>)> callback);
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
		std::string GetData();

		std::function<void(std::shared_ptr<GameObject>)> OnMouseOverFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnMouseLeaveFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnLeftClickFunction;
		std::function<void(std::shared_ptr<GameObject>)> OnRightClickFunction;

	private:
		bool _mouseIsOver;
		bool _hasMouseOverFired;
		bool _active;
		float activeWidth;
		float activeHeight;
		Vector2 activeOffset;
		int activeLayer;
	};
}