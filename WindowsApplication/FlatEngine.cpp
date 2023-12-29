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
	SceneManager FlatEngine::sceneManager;
	GameObjectManager FlatEngine::gameObjectManager;

	void FlatEngine::SetFocusedGameObjectIndex(int index)
	{
		FlatEngine::FocusedGameObjectIndex = index;
	}

	int FlatEngine::GetFocusedGameObjectIndex()
	{
		return FlatEngine::FocusedGameObjectIndex;
	}
}