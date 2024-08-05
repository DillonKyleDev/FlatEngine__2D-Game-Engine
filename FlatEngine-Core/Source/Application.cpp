#include "FlatEngine.h"
#include "Application.h"

namespace FlatEngine
{
	void Application::BeginRender()
	{
		FlatEngine::BeginImGuiRender();
	}

	void Application::EndRender()
	{
		FlatEngine::EndImGuiRender();
	}
}