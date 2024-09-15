#include "MappingContext.h"

#include <SDL.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine {

	MappingContext::MappingContext()
	{
		m_name = "";		
		m_inputsByBinding = std::map<std::string, std::shared_ptr<InputMapping>>();
		m_inputsByAction = std::map<std::string, std::shared_ptr<InputMapping>>();
	}

	MappingContext::~MappingContext()
	{
	}

	std::string MappingContext::GetData()
	{
		json jsonData = {
			{ "name", m_name }
		};

		for (std::pair<std::string, std::shared_ptr<InputMapping>> inputMapping : m_inputsByAction)
		{
			std::string keyString = inputMapping.second->keyCode;
			std::string inputAction = inputMapping.first;
			if (inputAction != "")
			{
				jsonData.emplace(keyString, inputAction);
			}
		}

		std::string data = jsonData.dump();		
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
		if (actionName != "")
		{
			std::shared_ptr<InputMapping> inputMap = std::make_shared<InputMapping>();
			inputMap->keyCode = keyBinding;
			inputMap->actionName = actionName;

			std::pair<std::string, std::shared_ptr<InputMapping>> bindingPair = { keyBinding, inputMap };
			std::pair<std::string, std::shared_ptr<InputMapping>> actionPair = { actionName, inputMap };

			// Add by binding
			if (m_inputsByBinding.count(keyBinding) > 0)
			{
				std::string oldActionName = m_inputsByBinding.at(keyBinding)->actionName;
				m_inputsByBinding.at(keyBinding) = inputMap;

				// erase old inputAction pair from m_inputsByAction if the action name has changed
				if (oldActionName != actionName)
				{
					m_inputsByAction.erase(oldActionName);
				}
			}
			else
			{
				m_inputsByBinding.emplace(bindingPair);
			}
			// Add by action
			if (m_inputsByAction.count(actionName) > 0)
			{
				m_inputsByAction.at(actionName) = inputMap;
			}
			else
			{
				m_inputsByAction.emplace(actionPair);
			}
		}
		else
		{
			std::string oldActionName = m_inputsByBinding.at(keyBinding)->actionName;
			m_inputsByBinding.erase(keyBinding);
			m_inputsByAction.erase(oldActionName);
		}
	}

	bool MappingContext::FireEvent(std::string keyBinding, SDL_Event event)
	{
		std::string actionName = "";

		if (m_inputsByBinding.count(keyBinding) > 0)
		{
			m_inputsByBinding.at(keyBinding)->event = event;
			m_inputsByBinding.at(keyBinding)->b_fired = true;
			return true;
		}

		return false;
	}

	void MappingContext::UnFireEvent(std::string keyBinding)
	{
		std::string actionName = "null";

		if (m_inputsByBinding.count(keyBinding) > 0)
		{			
			m_inputsByBinding.at(keyBinding)->b_fired = false;
		}
	}

	bool MappingContext::Fired(std::string actionName)
	{
		if (m_inputsByAction.count(actionName))
		{
			return m_inputsByAction.at(actionName)->b_fired;
		}
		else
		{
			return false;
		}
	}

	void MappingContext::ClearInputActionEvent(std::string keyBinding)
	{
		if (m_inputsByBinding.count(keyBinding) > 0)
		{
			m_inputsByBinding.at(keyBinding)->event = SDL_Event();
		}
	}

	// Get more detailed event information
	SDL_Event MappingContext::GetInputAction(std::string actionName)
	{
		SDL_Event inputEvent = SDL_Event();

		if (m_inputsByAction.count(actionName) > 0)
		{
			inputEvent = m_inputsByAction.at(actionName)->event;
		}

		return inputEvent;
	}

	bool MappingContext::ActionPressed(std::string actionName)
	{
		return GetInputAction(actionName).type != 0;
	}

	std::map<std::string, std::shared_ptr<InputMapping>> MappingContext::GetInputActions()
	{
		return m_inputsByAction;
	}
}