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
		m_keyBindings.emplace("Keyboard_UP", "");
		m_keyBindings.emplace("Keyboard_DOWN", "");
		m_keyBindings.emplace("Keyboard_LEFT", "");
		m_keyBindings.emplace("Keyboard_RIGHT", "");
		// Letters
		m_keyBindings.emplace("Keyboard_a", "");
		m_keyBindings.emplace("Keyboard_b", "");
		m_keyBindings.emplace("Keyboard_c", "");
		m_keyBindings.emplace("Keyboard_d", "");
		m_keyBindings.emplace("Keyboard_e", "");
		m_keyBindings.emplace("Keyboard_f", "");
		m_keyBindings.emplace("Keyboard_g", "");
		m_keyBindings.emplace("Keyboard_h", "");
		m_keyBindings.emplace("Keyboard_i", "");
		m_keyBindings.emplace("Keyboard_j", "");
		m_keyBindings.emplace("Keyboard_k", "");
		m_keyBindings.emplace("Keyboard_l", "");
		m_keyBindings.emplace("Keyboard_m", "");
		m_keyBindings.emplace("Keyboard_n", "");
		m_keyBindings.emplace("Keyboard_o", "");
		m_keyBindings.emplace("Keyboard_p", "");
		m_keyBindings.emplace("Keyboard_q", "");
		m_keyBindings.emplace("Keyboard_r", "");
		m_keyBindings.emplace("Keyboard_s", "");
		m_keyBindings.emplace("Keyboard_t", "");
		m_keyBindings.emplace("Keyboard_u", "");
		m_keyBindings.emplace("Keyboard_v", "");
		m_keyBindings.emplace("Keyboard_w", "");
		m_keyBindings.emplace("Keyboard_x", "");
		m_keyBindings.emplace("Keyboard_y", "");
		m_keyBindings.emplace("Keyboard_z", "");

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

			{ "Keyboard_up", m_keyBindings.at("Keyboard_up") },
			{ "Keyboard_down", m_keyBindings.at("Keyboard_down") },
			{ "Keyboard_left", m_keyBindings.at("Keyboard_left") },
			{ "Keyboard_right", m_keyBindings.at("Keyboard_right") },
			{ "Keyboard_space", m_keyBindings.at("Keyboard_space") },
			{ "Keyboard_leftCtrl", m_keyBindings.at("Keyboard_leftCtrl") },
			{ "Keyboard_leftShift", m_keyBindings.at("Keyboard_leftShift") },
			{ "Keyboard_leftAlt", m_keyBindings.at("Keyboard_leftAlt") },
			{ "Keyboard_rightCtrl", m_keyBindings.at("Keyboard_rightCtrl") },
			{ "Keyboard_rightShift", m_keyBindings.at("Keyboard_rightShift") },
			{ "Keyboard_rightAlt", m_keyBindings.at("Keyboard_rightAlt") },
			{ "Keyboard_capsLock", m_keyBindings.at("Keyboard_capsLock") },
			{ "Keyboard_numLock", m_keyBindings.at("Keyboard_numLock") },
			{ "Keyboard_backspace", m_keyBindings.at("Keyboard_backspace") },
			{ "Keyboard_escape", m_keyBindings.at("Keyboard_escape") },		
			{ "Keyboard_tab", m_keyBindings.at("Keyboard_tab") },

			{ "Keyboard_printScreen", m_keyBindings.at("Keyboard_printScreen") },
			{ "Keyboard_insert", m_keyBindings.at("Keyboard_insert") },
			{ "Keyboard_home", m_keyBindings.at("Keyboard_home") },
			{ "Keyboard_pageUp", m_keyBindings.at("Keyboard_pageUp") },
			{ "Keyboard_pageDown", m_keyBindings.at("Keyboard_pageDown") },
			{ "Keyboard_delete", m_keyBindings.at("Keyboard_delete") },

			{ "Keyboard_\\", m_keyBindings.at("Keyboard_\\") },
			{ "Keyboard_/", m_keyBindings.at("Keyboard_/") },
			{ "Keyboard_;", m_keyBindings.at("Keyboard_;") },
			{ "Keyboard_'", m_keyBindings.at("Keyboard_'") },
			{ "Keyboard_[", m_keyBindings.at("Keyboard_[") },
			{ "Keyboard_]", m_keyBindings.at("Keyboard_]") },
			{ "Keyboard_<", m_keyBindings.at("Keyboard_<") },
			{ "Keyboard_>", m_keyBindings.at("Keyboard_>") },
			{ "Keyboard_`", m_keyBindings.at("Keyboard_`") },
			{ "Keyboard_1", m_keyBindings.at("Keyboard_1") },
			{ "Keyboard_2", m_keyBindings.at("Keyboard_2") },
			{ "Keyboard_3", m_keyBindings.at("Keyboard_3") },
			{ "Keyboard_4", m_keyBindings.at("Keyboard_4") },
			{ "Keyboard_5", m_keyBindings.at("Keyboard_5") },
			{ "Keyboard_6", m_keyBindings.at("Keyboard_6") },
			{ "Keyboard_7", m_keyBindings.at("Keyboard_7") },
			{ "Keyboard_8", m_keyBindings.at("Keyboard_8") },
			{ "Keyboard_9", m_keyBindings.at("Keyboard_9") },
			{ "Keyboard_0", m_keyBindings.at("Keyboard_0") },
			{ "Keyboard_-", m_keyBindings.at("Keyboard_-") },
			{ "Keyboard_+", m_keyBindings.at("Keyboard_+") },

			{ "Keyboard_f1", m_keyBindings.at("Keyboard_f1") },
			{ "Keyboard_f2", m_keyBindings.at("Keyboard_f2") },
			{ "Keyboard_f3", m_keyBindings.at("Keyboard_f3") },
			{ "Keyboard_f4", m_keyBindings.at("Keyboard_f4") },
			{ "Keyboard_f5", m_keyBindings.at("Keyboard_f5") },
			{ "Keyboard_f6", m_keyBindings.at("Keyboard_f6") },
			{ "Keyboard_f7", m_keyBindings.at("Keyboard_f7") },
			{ "Keyboard_f8", m_keyBindings.at("Keyboard_f8") },
			{ "Keyboard_f9", m_keyBindings.at("Keyboard_f9") },
			{ "Keyboard_f10", m_keyBindings.at("Keyboard_f10") },
			{ "Keyboard_f11", m_keyBindings.at("Keyboard_f11") },
			{ "Keyboard_f12", m_keyBindings.at("Keyboard_f12") },

			{ "Keyboard_numPad_1", m_keyBindings.at("Keyboard_numPad_1") },
			{ "Keyboard_numPad_2", m_keyBindings.at("Keyboard_numPad_2") },
			{ "Keyboard_numPad_3", m_keyBindings.at("Keyboard_numPad_3") },
			{ "Keyboard_numPad_4", m_keyBindings.at("Keyboard_numPad_4") },
			{ "Keyboard_numPad_5", m_keyBindings.at("Keyboard_numPad_5") },
			{ "Keyboard_numPad_6", m_keyBindings.at("Keyboard_numPad_6") },
			{ "Keyboard_numPad_7", m_keyBindings.at("Keyboard_numPad_7") },
			{ "Keyboard_numPad_8", m_keyBindings.at("Keyboard_numPad_8") },
			{ "Keyboard_numPad_9", m_keyBindings.at("Keyboard_numPad_9") },
			{ "Keyboard_numPad_0", m_keyBindings.at("Keyboard_numPad_0") },
			{ "Keyboard_numPad_*", m_keyBindings.at("Keyboard_numPad_*") },
			{ "Keyboard_numPad_/", m_keyBindings.at("Keyboard_numPad_/") },
			{ "Keyboard_numPad_+", m_keyBindings.at("Keyboard_numPad_+") },
			{ "Keyboard_numPad_-", m_keyBindings.at("Keyboard_numPad_-") },
			{ "Keyboard_numPad_.", m_keyBindings.at("Keyboard_numPad_.") },
			{ "Keyboard_numPad_enter", m_keyBindings.at("Keyboard_numPad_enter") },

			{ "Keyboard_a", m_keyBindings.at("Keyboard_a") },
			{ "Keyboard_b", m_keyBindings.at("Keyboard_b") },
			{ "Keyboard_c", m_keyBindings.at("Keyboard_c") },
			{ "Keyboard_d", m_keyBindings.at("Keyboard_d") },
			{ "Keyboard_e", m_keyBindings.at("Keyboard_e") },
			{ "Keyboard_f", m_keyBindings.at("Keyboard_f") },
			{ "Keyboard_g", m_keyBindings.at("Keyboard_g") },
			{ "Keyboard_h", m_keyBindings.at("Keyboard_h") },
			{ "Keyboard_i", m_keyBindings.at("Keyboard_i") },
			{ "Keyboard_j", m_keyBindings.at("Keyboard_j") },
			{ "Keyboard_k", m_keyBindings.at("Keyboard_k") },
			{ "Keyboard_l", m_keyBindings.at("Keyboard_l") },
			{ "Keyboard_m", m_keyBindings.at("Keyboard_m") },
			{ "Keyboard_n", m_keyBindings.at("Keyboard_n") },
			{ "Keyboard_o", m_keyBindings.at("Keyboard_o") },
			{ "Keyboard_p", m_keyBindings.at("Keyboard_p") },
			{ "Keyboard_q", m_keyBindings.at("Keyboard_q") },
			{ "Keyboard_r", m_keyBindings.at("Keyboard_r") },
			{ "Keyboard_s", m_keyBindings.at("Keyboard_s") },
			{ "Keyboard_t", m_keyBindings.at("Keyboard_t") },
			{ "Keyboard_u", m_keyBindings.at("Keyboard_u") },
			{ "Keyboard_v", m_keyBindings.at("Keyboard_v") },
			{ "Keyboard_w", m_keyBindings.at("Keyboard_w") },
			{ "Keyboard_x", m_keyBindings.at("Keyboard_x") },
			{ "Keyboard_y", m_keyBindings.at("Keyboard_y") },
			{ "Keyboard_z", m_keyBindings.at("Keyboard_z") },
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