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

	void FlatEngine::SetFocusedGameObjectIndex(int index)
	{

		FlatEngine::FocusedGameObjectIndex = index;
	}

	int FlatEngine::GetFocusedGameObjectIndex()
	{
		return FlatEngine::FocusedGameObjectIndex;
	}
}