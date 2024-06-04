#include "PauseMenu.h"
#include "../FlatEngine.h"
#include "../MappingContext.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.cpp and 
// Add a new if branch for the script name in GameLoop()::Start() function


PauseMenu::PauseMenu()
{
	SetName("PauseMenu");
	mappingContext = nullptr;
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Start()
{
	FlatEngine::LogString("PauseMenu instantiated on: " + GetOwner()->GetName());
	mappingContext = FlatEngine::GetMappingContext("MC_CharacterContext.json");
	pauseScreen = GetOwner();
}

void PauseMenu::Update(float deltaTime)
{
	if (mappingContext != nullptr && mappingContext->Fired("IA_Pause_Keyboard"))
	{
		bool _isPaused = pauseScreen->IsActive();
		FlatEngine::LogString("Fired");
		pauseScreen->SetActive(!_isPaused);

		// Toggle pausing gameloop
		FlatEngine::PauseGame();
	}
}