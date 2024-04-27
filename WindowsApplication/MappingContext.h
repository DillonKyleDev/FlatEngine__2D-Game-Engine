#pragma once
#include <map>
#include "SDL.h"

namespace FlatEngine {

	class MappingContext
	{
	public:
		MappingContext();
		~MappingContext();

		void SetName(std::string name);
		std::string GetName();
		void AddKeyBinding(std::string keyBinding, std::string actionName);
		void SetKeyBindings(std::map<std::string, std::string> keyBindings);
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
		SDL_Event GetKeyBoundEvent(std::string keyBinding);
		std::map<std::string, std::string> GetKeyBindings();
		std::string GetData();

	private:
		std::string name;
		std::map<std::string, std::string> keyBindings;
		std::map<std::string, SDL_Event> inputActionBindings;
		std::map<std::string, bool> actionFiredBool;
	};
}