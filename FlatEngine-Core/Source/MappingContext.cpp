#include "MappingContext.h"

#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine {

	MappingContext::MappingContext()
	{
		m_name = "";
		m_keyBindings = std::map<std::string, std::string>();

		// XInput
		m_keyBindings.emplace("XInput_A", "");
		m_keyBindings.emplace("XInput_B", "");
		m_keyBindings.emplace("XInput_X", "");
		m_keyBindings.emplace("XInput_Y", "");
		m_keyBindings.emplace("XInput_LB", "");
		m_keyBindings.emplace("XInput_RB", "");
		m_keyBindings.emplace("XInput_ScreenShot", "");
		m_keyBindings.emplace("XInput_Start", "");
		m_keyBindings.emplace("XInput_LS", "");
		m_keyBindings.emplace("XInput_RS", "");
		m_keyBindings.emplace("XInput_Home", "");
		m_keyBindings.emplace("XInput_Tray", "");
		m_keyBindings.emplace("XInput_DPadUp", "");
		m_keyBindings.emplace("XInput_DPadDown", "");
		m_keyBindings.emplace("XInput_DPadLeft", "");
		m_keyBindings.emplace("XInput_DPadRight", "");
		m_keyBindings.emplace("XInput_LeftJoystickX", "");
		m_keyBindings.emplace("XInput_LeftJoystickY", "");
		m_keyBindings.emplace("XInput_RightJoystick", "");
		m_keyBindings.emplace("XInput_LT", "");
		m_keyBindings.emplace("XInput_RT", "");

		//// Keyboard + Mouse
		// Directions
		m_keyBindings.emplace("SDLK_UP", "");
		m_keyBindings.emplace("SDLK_DOWN", "");
		m_keyBindings.emplace("SDLK_LEFT", "");
		m_keyBindings.emplace("SDLK_RIGHT", "");
		// Letters
		m_keyBindings.emplace("SDLK_a", "");
		m_keyBindings.emplace("SDLK_b", "");
		m_keyBindings.emplace("SDLK_c", "");
		m_keyBindings.emplace("SDLK_d", "");
		m_keyBindings.emplace("SDLK_e", "");
		m_keyBindings.emplace("SDLK_f", "");
		m_keyBindings.emplace("SDLK_g", "");
		m_keyBindings.emplace("SDLK_h", "");
		m_keyBindings.emplace("SDLK_i", "");
		m_keyBindings.emplace("SDLK_j", "");
		m_keyBindings.emplace("SDLK_k", "");
		m_keyBindings.emplace("SDLK_l", "");
		m_keyBindings.emplace("SDLK_m", "");
		m_keyBindings.emplace("SDLK_n", "");
		m_keyBindings.emplace("SDLK_o", "");
		m_keyBindings.emplace("SDLK_p", "");
		m_keyBindings.emplace("SDLK_q", "");
		m_keyBindings.emplace("SDLK_r", "");
		m_keyBindings.emplace("SDLK_s", "");
		m_keyBindings.emplace("SDLK_t", "");
		m_keyBindings.emplace("SDLK_u", "");
		m_keyBindings.emplace("SDLK_v", "");
		m_keyBindings.emplace("SDLK_w", "");
		m_keyBindings.emplace("SDLK_x", "");
		m_keyBindings.emplace("SDLK_y", "");
		m_keyBindings.emplace("SDLK_z", "");

		m_inputActionBindings = std::map<std::string, SDL_Event>();
		m_actionFiredBools = std::map<std::string, bool>();
	}

	MappingContext::~MappingContext()
	{
	}

	std::string MappingContext::GetData()
	{
		json jsonData = {
			{ "name", m_name },
			{ "XInput_A", m_keyBindings.at("XInput_A") },
			{ "XInput_B", m_keyBindings.at("XInput_B") },
			{ "XInput_X", m_keyBindings.at("XInput_X") },
			{ "XInput_Y", m_keyBindings.at("XInput_Y") },
			{ "XInput_LB", m_keyBindings.at("XInput_LB") },
			{ "XInput_RB", m_keyBindings.at("XInput_RB") },
			{ "XInput_ScreenShot", m_keyBindings.at("XInput_ScreenShot") },
			{ "XInput_Start", m_keyBindings.at("XInput_Start") },
			{ "XInput_LS", m_keyBindings.at("XInput_LS") },
			{ "XInput_RS", m_keyBindings.at("XInput_RS") },
			{ "XInput_Home", m_keyBindings.at("XInput_Home") },
			{ "XInput_Tray", m_keyBindings.at("XInput_Tray") },
			{ "XInput_DPadUp", m_keyBindings.at("XInput_DPadUp") },
			{ "XInput_DPadDown", m_keyBindings.at("XInput_DPadDown") },
			{ "XInput_DPadLeft", m_keyBindings.at("XInput_DPadLeft") },
			{ "XInput_DPadRight", m_keyBindings.at("XInput_DPadRight") },
			{ "XInput_LeftJoystickX", m_keyBindings.at("XInput_LeftJoystickX") },
			{ "XInput_LeftJoystickY", m_keyBindings.at("XInput_LeftJoystickY") },
			{ "XInput_RightJoystick", m_keyBindings.at("XInput_RightJoystick") },
			{ "XInput_LT", m_keyBindings.at("XInput_LT") },
			{ "XInput_RT", m_keyBindings.at("XInput_RT") },
			{ "SDLK_UP", m_keyBindings.at("SDLK_UP") },
			{ "SDLK_DOWN", m_keyBindings.at("SDLK_DOWN") },
			{ "SDLK_LEFT", m_keyBindings.at("SDLK_LEFT") },
			{ "SDLK_RIGHT", m_keyBindings.at("SDLK_RIGHT") },
			{ "SDLK_a", m_keyBindings.at("SDLK_a") },
			{ "SDLK_b", m_keyBindings.at("SDLK_b") },
			{ "SDLK_c", m_keyBindings.at("SDLK_c") },
			{ "SDLK_d", m_keyBindings.at("SDLK_d") },
			{ "SDLK_e", m_keyBindings.at("SDLK_e") },
			{ "SDLK_f", m_keyBindings.at("SDLK_f") },
			{ "SDLK_g", m_keyBindings.at("SDLK_g") },
			{ "SDLK_h", m_keyBindings.at("SDLK_h") },
			{ "SDLK_i", m_keyBindings.at("SDLK_i") },
			{ "SDLK_j", m_keyBindings.at("SDLK_j") },
			{ "SDLK_k", m_keyBindings.at("SDLK_k") },
			{ "SDLK_l", m_keyBindings.at("SDLK_l") },
			{ "SDLK_m", m_keyBindings.at("SDLK_m") },
			{ "SDLK_n", m_keyBindings.at("SDLK_n") },
			{ "SDLK_o", m_keyBindings.at("SDLK_o") },
			{ "SDLK_p", m_keyBindings.at("SDLK_p") },
			{ "SDLK_q", m_keyBindings.at("SDLK_q") },
			{ "SDLK_r", m_keyBindings.at("SDLK_r") },
			{ "SDLK_s", m_keyBindings.at("SDLK_s") },
			{ "SDLK_t", m_keyBindings.at("SDLK_t") },
			{ "SDLK_u", m_keyBindings.at("SDLK_u") },
			{ "SDLK_v", m_keyBindings.at("SDLK_v") },
			{ "SDLK_w", m_keyBindings.at("SDLK_w") },
			{ "SDLK_x", m_keyBindings.at("SDLK_x") },
			{ "SDLK_y", m_keyBindings.at("SDLK_y") },
			{ "SDLK_z", m_keyBindings.at("SDLK_z") },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void MappingContext::SetName(std::string contextName)
	{
		m_name = contextName;
	}

	std::string MappingContext::GetName()
	{
		return m_name;
	}

	void MappingContext::SetPath(std::string filepath)
	{
		m_path = filepath;
	}

	std::string MappingContext::GetPath()
	{
		return m_path;
	}

	void MappingContext::AddKeyBinding(std::string keyBinding, std::string actionName)
	{
		if (m_keyBindings.count(keyBinding) == 0)
		{
			m_keyBindings.emplace(keyBinding, actionName);
		}
		else
		{
			m_keyBindings.at(keyBinding) = actionName;
		}

		if (m_actionFiredBools.count(actionName) == 0)
		{
			m_actionFiredBools.emplace(actionName, false);
		}
		else
		{
			m_actionFiredBools.at(actionName) = false;
		}
	}

	void MappingContext::RemoveKeyBinding(std::string keyBinding)
	{
		m_keyBindings.at(keyBinding) = "";
	}

	void MappingContext::SetKeyBindings(std::map<std::string, std::string> newKeyBindings)
	{
		m_keyBindings = newKeyBindings;
		CreateInputActionBindings();
	}

	void MappingContext::SetKeyBinding(std::string keyBinding, std::string actionName)
	{
		// Erase old Input Action binding
		if (m_inputActionBindings.count(m_keyBindings.at(keyBinding)) > 0)
		{
			m_inputActionBindings.erase(m_keyBindings.at(keyBinding));
		}

		m_keyBindings.at(keyBinding) = actionName;
	}

	std::string MappingContext::GetKeyBinding(std::string keyBinding)
	{
		if (m_keyBindings.count(keyBinding) > 0)
		{
			return m_keyBindings.at(keyBinding);
		}
		else
		{
			return "null";
		}
	}

	void MappingContext::CreateInputActionBindings()
	{
		SDL_Event emptyEvent = SDL_Event();
		
		for (std::map<std::string, std::string>::iterator iterator = m_keyBindings.begin(); iterator != m_keyBindings.end(); iterator++)
		{
			if (iterator->second != "")
			{
				m_inputActionBindings.emplace(iterator->second, emptyEvent);
				m_actionFiredBools.emplace(iterator->second, false);
			}
		}
	}

	void MappingContext::AddInputAction(std::string keyBinding, std::string actionName)
	{		
		if (m_keyBindings.at(keyBinding) != "")
		{
			m_inputActionBindings.erase(m_keyBindings.at(keyBinding));
		}

		SDL_Event emptyEvent = SDL_Event();	
		m_keyBindings.at(keyBinding) = actionName;		
		m_inputActionBindings.emplace(actionName, emptyEvent);
	}

	void MappingContext::OnInputEvent(std::string keyBinding, SDL_Event event)
	{
		std::string actionName = "null";

		if (m_keyBindings.count(keyBinding) > 0)
		{
			actionName = m_keyBindings.at(keyBinding);
		}
		if (m_inputActionBindings.count(actionName) > 0)
		{
			m_inputActionBindings.at(actionName) = event;
		}
		if (actionName != "null")
		{
			FireEvent(keyBinding);
		}
	}

	void MappingContext::FireEvent(std::string keyBinding)
	{
		std::string actionName = "";

		if (m_keyBindings.count(keyBinding) > 0)
		{
			actionName = m_keyBindings.at(keyBinding);
		}
		if (m_actionFiredBools.count(actionName) > 0)
		{
			m_actionFiredBools.at(actionName) = true;
		}
	}

	void MappingContext::UnFireEvent(std::string keyBinding)
	{
		std::string actionName = "null";

		if (m_keyBindings.count(keyBinding) > 0)
		{
			actionName = m_keyBindings.at(keyBinding);
		}
		if (actionName != "null")
		{
			m_actionFiredBools.at(actionName) = false;
		}
	}

	bool MappingContext::Fired(std::string actionName)
	{
		if (m_actionFiredBools.count(actionName) > 0)
		{
			return m_actionFiredBools.at(actionName);
		}
		else 
		{
			return false;
		}
	}

	void MappingContext::ClearInputActionEvents()
	{
		SDL_Event emptyEvent = SDL_Event();
		
		for (std::map<std::string, SDL_Event>::iterator iterator = m_inputActionBindings.begin(); iterator != m_inputActionBindings.end(); iterator++)
		{
			iterator->second = emptyEvent;
		}
	}

	void MappingContext::ClearInputActionEvent(std::string keyBinding)
	{
		SDL_Event emptyEvent = SDL_Event();

		if (m_keyBindings.count(keyBinding) > 0)
		{
			if (m_inputActionBindings.count(m_keyBindings.at(keyBinding)) > 0)
			{
				m_inputActionBindings.at(m_keyBindings.at(keyBinding)) = emptyEvent;
			}
		}
	}

	SDL_Event MappingContext::GetInputAction(std::string actionName)
	{
		SDL_Event inputEvent = SDL_Event();

		if (m_inputActionBindings.count(actionName) > 0)
		{
			inputEvent = m_inputActionBindings.at(actionName);
		}

		return inputEvent;
	}

	bool MappingContext::ActionPressed(std::string actionName)
	{
		return GetInputAction(actionName).type != 0;
	}

	SDL_Event MappingContext::GetKeyBoundEvent(std::string keyBinding)
	{
		SDL_Event inputEvent = SDL_Event();

		if (m_keyBindings.count(keyBinding) > 0)
		{
			inputEvent = m_inputActionBindings.at(m_keyBindings.at(keyBinding));
		}

		return inputEvent;
	}

	std::map<std::string, std::string> MappingContext::GetKeyBindings()
	{
		return m_keyBindings;
	}
}