#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Texture.h"
#include "Vector2.h"



namespace FlatEngine
{
	typedef void (*CallbackFunction)(int params);

	class Button : public Component
	{
	public:
		Button(long parentID);
		~Button();
		
		void OnMouseOver(CallbackFunction onMouseOver(int params));
		void OnMouseLeave(CallbackFunction onMouseLeave(int params));
		void OnMouseLeftClick(CallbackFunction onLeftClick(int params));
		void OnMouseRightClick(CallbackFunction onRightClick(int params));
		void SetActive(bool _active);
		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		void SetAttachedScript(std::string script);
		bool IsActive();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
		std::string GetAttachedScript();
		void SetMouseIsOver(bool _isOver);
		void SetIsOverFired(bool _fired);
		bool MouseIsOver();
		std::string GetData();

	private:
		bool _mouseIsOver;
		bool _hasMouseOverFired;
		bool _active;
		float activeWidth;
		float activeHeight;
		Vector2 activeOffset;

		std::string attachedScript;

		CallbackFunction OnMouseOverFunction;
		CallbackFunction OnMouseLeaveFunction;
		CallbackFunction OnLeftClickFunction;
		CallbackFunction OnRightClickFunction;
	};
}