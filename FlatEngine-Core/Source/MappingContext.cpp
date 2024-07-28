#include "FlatEngine.h"
#include "MappingContext.h"


namespace FlatEngine {

	MappingContext::MappingContext()
	{
		name = "";
		keyBindings = std::map<std::string, std::string>();

		// XInput
		keyBindings.emplace("XInput_A", "");
		keyBindings.emplace("XInput_B", "");
		keyBindings.emplace("XInput_X", "");
		keyBindings.emplace("XInput_Y", "");	
		keyBindings.emplace("XInput_LB", "");
		keyBindings.emplace("XInput_RB", "");
		keyBindings.emplace("XInput_ScreenShot", "");
		keyBindings.emplace("XInput_Start", "");
		keyBindings.emplace("XInput_LS", "");
		keyBindings.emplace("XInput_RS", "");
		keyBindings.emplace("XInput_Home", "");
		keyBindings.emplace("XInput_Tray", "");
		keyBindings.emplace("XInput_DPadUp", "");
		keyBindings.emplace("XInput_DPadDown", "");
		keyBindings.emplace("XInput_DPadLeft", "");
		keyBindings.emplace("XInput_DPadRight", "");
		keyBindings.emplace("XInput_LeftJoystickX", "");
		keyBindings.emplace("XInput_LeftJoystickY", "");
		keyBindings.emplace("XInput_RightJoystick", "");
		keyBindings.emplace("XInput_LT", "");
		keyBindings.emplace("XInput_RT", "");

		//// Keyboard + Mouse
		// Directions
		keyBindings.emplace("SDLK_UP", "");
		keyBindings.emplace("SDLK_DOWN", "");
		keyBindings.emplace("SDLK_LEFT", "");
		keyBindings.emplace("SDLK_RIGHT", "");
		// Letters
		keyBindings.emplace("SDLK_a", "");
		keyBindings.emplace("SDLK_b", "");
		keyBindings.emplace("SDLK_c", "");
		keyBindings.emplace("SDLK_d", "");
		keyBindings.emplace("SDLK_e", "");
		keyBindings.emplace("SDLK_f", "");
		keyBindings.emplace("SDLK_g", "");
		keyBindings.emplace("SDLK_h", "");
		keyBindings.emplace("SDLK_i", "");
		keyBindings.emplace("SDLK_j", "");
		keyBindings.emplace("SDLK_k", "");
		keyBindings.emplace("SDLK_l", "");
		keyBindings.emplace("SDLK_m", "");
		keyBindings.emplace("SDLK_n", "");
		keyBindings.emplace("SDLK_o", "");
		keyBindings.emplace("SDLK_p", "");
		keyBindings.emplace("SDLK_q", "");
		keyBindings.emplace("SDLK_r", "");
		keyBindings.emplace("SDLK_s", "");
		keyBindings.emplace("SDLK_t", "");
		keyBindings.emplace("SDLK_u", "");
		keyBindings.emplace("SDLK_v", "");
		keyBindings.emplace("SDLK_w", "");
		keyBindings.emplace("SDLK_x", "");
		keyBindings.emplace("SDLK_y", "");
		keyBindings.emplace("SDLK_z", "");

		inputActionBindings = std::map<std::string, SDL_Event>();
		actionFiredBool = std::map<std::string, bool>();
	}

	MappingContext::~MappingContext()
	{
	}

	void MappingContext::SetName(std::string contextName)
	{
		name = contextName;
	}

	std::string MappingContext::GetName()
	{
		return name;
	}

	void MappingContext::SetPath(std::string filepath)
	{
		path = filepath;
	}

	std::string MappingContext::GetPath()
	{
		return path;
	}

	void MappingContext::AddKeyBinding(std::string keyBinding, std::string actionName)
	{
		if (keyBindings.count(keyBinding) == 0)
			keyBindings.emplace(keyBinding, actionName);
		else
			keyBindings.at(keyBinding) = actionName;

		if (actionFiredBool.count(actionName) == 0)
			actionFiredBool.emplace(actionName, false);
		else
			actionFiredBool.at(actionName) = false;
	}

	void MappingContext::RemoveKeyBinding(std::string keyBinding)
	{
		keyBindings.at(keyBinding) = "";
	}

	void MappingContext::SetKeyBindings(std::map<std::string, std::string> newKeyBindings)
	{
		keyBindings = newKeyBindings;
		CreateInputActionBindings();
	}

	void MappingContext::SetKeyBinding(std::string keyBinding, std::string actionName)
	{
		// Erase old Input Action binding
		if (inputActionBindings.count(keyBindings.at(keyBinding)) > 0)
			inputActionBindings.erase(keyBindings.at(keyBinding));
		keyBindings.at(keyBinding) = actionName;
	}

	std::string MappingContext::GetKeyBinding(std::string keyBinding)
	{
		if (keyBindings.count(keyBinding) > 0)
			return keyBindings.at(keyBinding);
		else
			return "null";
	}

	void MappingContext::CreateInputActionBindings()
	{
		std::map<std::string, std::string>::iterator iterator = keyBindings.begin();
		SDL_Event emptyEvent = SDL_Event();

		// If it's not empty, set the keyBinding InputAction value to an inputActionBindings key to be looked up later
		while (iterator != keyBindings.end())
		{
			if (iterator->second != "")
			{
				inputActionBindings.emplace(iterator->second, emptyEvent);
				actionFiredBool.emplace(iterator->second, false);
			}
			iterator++;
		}
	}

	void MappingContext::AddInputAction(std::string keyBinding, std::string actionName)
	{
		// Remove old Input Action on the old keyBinding if there was one
		if (keyBindings.at(keyBinding) != "")
			inputActionBindings.erase(keyBindings.at(keyBinding));

		SDL_Event emptyEvent = SDL_Event();
		// Update the keyBinding with new Input Action name
		keyBindings.at(keyBinding) = actionName;
		// Create new Input Action with new Input Action name
		inputActionBindings.emplace(actionName, emptyEvent);
	}

	void MappingContext::OnInputEvent(std::string keyBinding, SDL_Event event)
	{
		std::string actionName = "null";
		if (keyBindings.count(keyBinding) > 0)
			actionName = keyBindings.at(keyBinding);
		if (inputActionBindings.count(actionName) > 0)
			inputActionBindings.at(actionName) = event;
		if (actionName != "null")
			FireEvent(keyBinding);
	}

	void MappingContext::FireEvent(std::string keyBinding)
	{
		std::string actionName = "";
		if (keyBindings.count(keyBinding) > 0)
			actionName = keyBindings.at(keyBinding);
		if (actionFiredBool.count(actionName) > 0)
			actionFiredBool.at(actionName) = true;
	}

	void MappingContext::UnFireEvent(std::string keyBinding)
	{
		std::string actionName = "null";
		if (keyBindings.count(keyBinding) > 0)
			actionName = keyBindings.at(keyBinding);
		if (actionName != "null")
			actionFiredBool.at(actionName) = false;
	}

	bool MappingContext::Fired(std::string actionName)
	{
		if (actionFiredBool.count(actionName) > 0)
			return actionFiredBool.at(actionName);
		else 
			return false;
	}

	void MappingContext::ClearInputActionEvents()
	{
		std::map<std::string, SDL_Event>::iterator iterator = inputActionBindings.begin();
		SDL_Event emptyEvent = SDL_Event();

		//// If it's not empty, set the keyBinding InputAction value to an inputActionBindings key to be looked up later
		while (iterator != inputActionBindings.end())
		{
			iterator->second = emptyEvent;
			iterator++;
		}
	}

	void MappingContext::ClearInputActionEvent(std::string keyBinding)
	{
		std::map<std::string, std::string>::iterator iterator = keyBindings.begin();
		SDL_Event emptyEvent = SDL_Event();

		//// If it's not empty, set the keyBinding InputAction value to an inputActionBindings key to be looked up later
		while (iterator != keyBindings.end())
		{
			if (iterator->first == keyBinding && inputActionBindings.count(iterator->second) > 0)
			{		
				inputActionBindings.at(iterator->second) = emptyEvent;
				return;
			}
				
			iterator++;
		}
	}

	SDL_Event MappingContext::GetInputAction(std::string actionName)
	{
		SDL_Event emptyEvent = SDL_Event();

		if (inputActionBindings.count(actionName) > 0)
			emptyEvent = inputActionBindings.at(actionName);

		return emptyEvent;
	}

	SDL_Event MappingContext::GetKeyBoundEvent(std::string keyBinding)
	{
		SDL_Event emptyEvent = SDL_Event();

		if (keyBindings.count(keyBinding) > 0)
			emptyEvent = inputActionBindings.at(keyBindings.at(keyBinding));

		return emptyEvent;
	}

	std::map<std::string, std::string> MappingContext::GetKeyBindings()
	{
		return keyBindings;
	}

	std::string MappingContext::GetData()
	{
		json jsonData = {
			{ "name", name },
			{ "XInput_A", keyBindings.at("XInput_A") },
			{ "XInput_B", keyBindings.at("XInput_B") },
			{ "XInput_X", keyBindings.at("XInput_X") },
			{ "XInput_Y", keyBindings.at("XInput_Y") },
			{ "XInput_LB", keyBindings.at("XInput_LB") },
			{ "XInput_RB", keyBindings.at("XInput_RB") },
			{ "XInput_ScreenShot", keyBindings.at("XInput_ScreenShot") },
			{ "XInput_Start", keyBindings.at("XInput_Start") },
			{ "XInput_LS", keyBindings.at("XInput_LS") },
			{ "XInput_RS", keyBindings.at("XInput_RS") },
			{ "XInput_Home", keyBindings.at("XInput_Home") },
			{ "XInput_Tray", keyBindings.at("XInput_Tray") },
			{ "XInput_DPadUp", keyBindings.at("XInput_DPadUp") },
			{ "XInput_DPadDown", keyBindings.at("XInput_DPadDown") },
			{ "XInput_DPadLeft", keyBindings.at("XInput_DPadLeft") },
			{ "XInput_DPadRight", keyBindings.at("XInput_DPadRight") },
			{ "XInput_LeftJoystickX", keyBindings.at("XInput_LeftJoystickX") },
			{ "XInput_LeftJoystickY", keyBindings.at("XInput_LeftJoystickY") },
			{ "XInput_RightJoystick", keyBindings.at("XInput_RightJoystick") },
			{ "XInput_LT", keyBindings.at("XInput_LT") },
			{ "XInput_RT", keyBindings.at("XInput_RT") },
			{ "SDLK_UP", keyBindings.at("SDLK_UP") },
			{ "SDLK_DOWN", keyBindings.at("SDLK_DOWN") },
			{ "SDLK_LEFT", keyBindings.at("SDLK_LEFT") },
			{ "SDLK_RIGHT", keyBindings.at("SDLK_RIGHT") },
			{ "SDLK_a", keyBindings.at("SDLK_a") },
			{ "SDLK_b", keyBindings.at("SDLK_b") },
			{ "SDLK_c", keyBindings.at("SDLK_c") },
			{ "SDLK_d", keyBindings.at("SDLK_d") },
			{ "SDLK_e", keyBindings.at("SDLK_e") },
			{ "SDLK_f", keyBindings.at("SDLK_f") },
			{ "SDLK_g", keyBindings.at("SDLK_g") },
			{ "SDLK_h", keyBindings.at("SDLK_h") },
			{ "SDLK_i", keyBindings.at("SDLK_i") },
			{ "SDLK_j", keyBindings.at("SDLK_j") },
			{ "SDLK_k", keyBindings.at("SDLK_k") },
			{ "SDLK_l", keyBindings.at("SDLK_l") },
			{ "SDLK_m", keyBindings.at("SDLK_m") },
			{ "SDLK_n", keyBindings.at("SDLK_n") },
			{ "SDLK_o", keyBindings.at("SDLK_o") },
			{ "SDLK_p", keyBindings.at("SDLK_p") },
			{ "SDLK_q", keyBindings.at("SDLK_q") },
			{ "SDLK_r", keyBindings.at("SDLK_r") },
			{ "SDLK_s", keyBindings.at("SDLK_s") },
			{ "SDLK_t", keyBindings.at("SDLK_t") },
			{ "SDLK_u", keyBindings.at("SDLK_u") },
			{ "SDLK_v", keyBindings.at("SDLK_v") },
			{ "SDLK_w", keyBindings.at("SDLK_w") },
			{ "SDLK_x", keyBindings.at("SDLK_x") },
			{ "SDLK_y", keyBindings.at("SDLK_y") },
			{ "SDLK_z", keyBindings.at("SDLK_z") },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}