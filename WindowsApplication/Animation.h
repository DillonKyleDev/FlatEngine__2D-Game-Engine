#pragma once
#include "Component.h"
#include "GameObject.h"



namespace FlatEngine
{
	class Animation : public Component
	{
	public:
		Animation(long myID, long parentID);
		~Animation();

		void AddFrame();
		void Play();
		void Stop();

		void SetTicksPerFrame(float ticksPerFrame);
		float GetTicksPerFrame();
		std::vector<std::shared_ptr<GameObject>> GetFrames();
		std::string GetData();

	private:
		float ticksPerFrame;
		std::vector<std::shared_ptr<GameObject>> frames;
	};
}

