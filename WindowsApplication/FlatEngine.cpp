#include "FlatEngine.h"


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{
	//Variables
	int FlatEngine::FocusedGameObjectIndex = -1;
	FlatEngine::SceneManager *FlatEngine::sceneManager = new FlatEngine::SceneManager();
	FlatEngine::Logger *FlatEngine::logger = new FlatEngine::Logger();

	void FlatEngine::SetFocusedGameObjectIndex(int index)
	{
		FlatEngine::FocusedGameObjectIndex = index;
	}

	int FlatEngine::GetFocusedGameObjectIndex()
	{
		return FlatEngine::FocusedGameObjectIndex;
	}
}