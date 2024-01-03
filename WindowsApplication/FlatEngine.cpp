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
	FlatEngine::SceneManager* FlatEngine::sceneManager = new FlatEngine::SceneManager();
	FlatEngine::Logger* FlatEngine::logger = new FlatEngine::Logger();

	void FlatEngine::SetFocusedGameObjectIndex(int index)
	{
		FlatEngine::FocusedGameObjectIndex = index;
	}

	int FlatEngine::GetFocusedGameObjectIndex()
	{
		return FlatEngine::FocusedGameObjectIndex;
	}


	// Logging
	void FlatEngine::LogString(std::string line)
	{
		logger->LogString(line);
	}

	void FlatEngine::LogFloat(float var, std::string line)
	{
		logger->LogFloat(var, line);
	}

	void FlatEngine::LogInt(int var, std::string line)
	{
		logger->LogInt(var, line);
	}

	void FlatEngine::LogVector2(Vector2 vector, std::string line)
	{
		logger->LogVector2(vector, line);
	}

	void FlatEngine::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint)
	{
		logger->DrawRectangle(startingPoint, endingPoint);
	}

	void FlatEngine::DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint)
	{
		logger->DrawRectangle(startingPoint, endingPoint);
	}
}