#pragma once
#include <map>
#include "SDL.h"

namespace FlatEngine {

	enum XInputButtons {
		A,
		B,
		X,
		Y,
		LB,
		RB,
		ScreenShot,
		Start,
		LS,
		RS,
		Home,
		Tray,
	};
	enum XInputHats {
		Hat_Up = 1,
		Hat_Down = 4,
		Hat_Left = 8,
		Hat_Right = 2,

	};
	enum XInputAxis {
		LeftXAxis,
		LeftYAxis,
		RightXAxis,
		RightYAxis,
		LT,
		RT
	};

	class MappingContext
	{
	public:
		MappingContext();
		~MappingContext();

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		void AddKeyBinding(std::string keyBinding, std::string actionName);
		void RemoveKeyBinding(std::string keyBinding);
		void SetKeyBindings(std::map<std::string, std::string> keyBindings);
		void SetKeyBinding(std::string keyBinding, std::string actionName);
		std::string GetKeyBinding(std::string keyBinding);
		void CreateInputActionBindings();
		void AddInputAction(std::string keyBinding, std::string actionName);
		void OnInputEvent(std::string keyBinding, SDL_Event event);
		void FireEvent(std::string actionName);
		void UnFireEvent(std::string actionName);
		bool Fired(std::string actionName);
		void ClearInputActionEvents();
		void ClearInputActionEvent(std::string keyBinding);
		SDL_Event GetInputAction(std::string actionName);
		bool ActionPressed(std::string actionName);
		SDL_Event GetKeyBoundEvent(std::string keyBinding);
		std::map<std::string, std::string> GetKeyBindings();
		std::string GetData();

	private:
		std::string name;
		std::string path;
		std::map<std::string, std::string> keyBindings;
		std::map<std::string, SDL_Event> inputActionBindings;
		std::map<std::string, bool> actionFiredBool;
	};
}