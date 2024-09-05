#pragma once
#include "Component.h"


namespace FlatEngine
{
	class Script : public Component
	{
	public:
		Script(long myID = -1, long parentID = -1);
		~Script();

		std::string GetData();
		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();

	private:
		std::string m_attachedScript;
	};
}
