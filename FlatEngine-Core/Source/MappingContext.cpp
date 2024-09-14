#include "MappingContext.h"

#include <SDL.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine {

	MappingContext::MappingContext()
	{
		m_name = "";
		m_keyBindings = std::map<std::string, std::string>();

		for (std::pair<long, std::string> inputKeycode : F_MappedKeyboardCodes)
		{
			m_keyBindings.emplace(inputKeycode.second, "");
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputButtonCodes)
		{
			m_keyBindings.emplace(inputKeycode.second, "");
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputDPadCodes)
		{
			m_keyBindings.emplace(inputKeycode.second, "");
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputAnalogCodes)
		{
			m_keyBindings.emplace(inputKeycode.second, "");
		}

		m_inputActionBindings = std::map<std::string, SDL_Event>();
		m_actionFiredBools = std::map<std::string, bool>();
	}

	MappingContext::~MappingContext()
	{
	}

	std::string MappingContext::GetData()
	{
		json jsonData = {
			{ "name", m_name }
		};
		
		// Keyboard
		for (std::pair<long, std::string> keyboardCode : F_MappedKeyboardCodes)
		{
			std::string keyString = keyboardCode.second;
			std::string inputAction = m_keyBindings.at(keyString);
			jsonData.emplace(keyString, inputAction);
		}
		// XInput Buttons
		for (std::pair<long, std::string> xInputButtonCode : F_MappedXInputButtonCodes)
		{
			std::string keyString = xInputButtonCode.second;
			std::string inputAction = m_keyBindings.at(keyString);
			jsonData.emplace(keyString, inputAction);
		}
		// XInput DPad
		for (std::pair<long, std::string> xInputDPadCode : F_MappedXInputDPadCodes)
		{
			std::string keyString = xInputDPadCode.second;
			std::string inputAction = m_keyBindings.at(keyString);

			jsonData.emplace(keyString, inputAction);
		}
		// XInput Analogs
		for (std::pair<long, std::string> xInputAnalogCode : F_MappedXInputAnalogCodes)
		{
			std::string keyString = xInputAnalogCode.second;
			std::string inputAction = m_keyBindings.at(keyString);

			jsonData.emplace(keyString, inputAction);
		}

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

		if (m_keyBindings.count(keyBinding) > 0 && m_inputActionBindings.count(m_keyBindings.at(keyBinding)) > 0)
		{
			m_inputActionBindings.at(m_keyBindings.at(keyBinding)) = emptyEvent;
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

		if (m_keyBindings.count(keyBinding) > 0 && m_inputActionBindings.count(m_keyBindings.at(keyBinding)) > 0)
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