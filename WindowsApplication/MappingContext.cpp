#include "FlatEngine.h"
#include "MappingContext.h"


namespace FlatEngine {

	MappingContext::MappingContext()
	{
		name = "";
		keyBindings = std::map<std::string, std::string>();
		// Directions
		keyBindings.emplace("SDLK_UP", "IA_Jump");
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

	void MappingContext::SetKeyBindings(std::map<std::string, std::string> newKeyBindings)
	{
		keyBindings = newKeyBindings;
		CreateInputActionBindings();
	}

	std::string MappingContext::GetKeyBinding(std::string keyBinding)
	{
		return keyBindings.at(keyBinding);
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
		SDL_Event emptyEvent = SDL_Event();
		keyBindings.at(keyBinding) = actionName;
		inputActionBindings.emplace(actionName, emptyEvent);
	}

	void MappingContext::OnInputEvent(std::string keyBinding, SDL_Event event)
	{
		std::string actionName = keyBindings.at(keyBinding);
		inputActionBindings.at(actionName) = event;
		FireEvent(keyBinding);
		//LogString("Firing event");
	}

	void MappingContext::FireEvent(std::string keyBinding)
	{
		std::string actionName = "";
		if (keyBindings.count(keyBinding) > 0)
			actionName = keyBindings.at(keyBinding);
		actionFiredBool.at(actionName) = true;
	}

	void MappingContext::UnFireEvent(std::string keyBinding)
	{
		std::string actionName = keyBindings.at(keyBinding);
		actionFiredBool.at(actionName) = false;
	}

	bool MappingContext::Fired(std::string actionName)
	{
		return actionFiredBool.at(actionName);
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
			if (iterator->first == keyBinding)
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