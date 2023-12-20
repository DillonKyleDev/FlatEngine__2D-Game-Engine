#pragma once


namespace FlatEngine
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		//virtual void OnUpdate();
	};

	//Application* CreateApplication();
}