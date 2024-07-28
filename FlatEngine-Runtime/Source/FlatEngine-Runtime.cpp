#include <FlatEngine.h>

int main(int argc, char* args[])
{
	////Start up SDL and create window
	//if (!init())
	//{
	//	printf("Failed to initialize!\n");
	//}
	//else
	//{
	//	FlatEngine::FlatGui::SetupImGui();
	//	// Create icons
	//	FlatEngine::FlatGui::CreateIcons();

	//	//Load media
	//	if (!loadMedia())
	//	{
	//		printf("Failed to load media!\n");
	//	}
	//	else
	//	{
	//		//While application is running
	//		while (!_hasQuit)
	//		{
	//			FlatEngine::Run(_hasQuit);
	//		}
	//	}
	//}

	//close();
	FlatEngine::LogString("Hello from Runtime!");
	return 0;
}