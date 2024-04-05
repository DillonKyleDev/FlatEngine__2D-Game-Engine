#pragma once
#include "Component.h"


namespace FlatEngine
{
	class GameObject;

	class Animation : public Component
	{
	public:
		Animation(long myID = -1, long parentID = -1);
		~Animation();

		void AddFrame();
		void Play();
		void Stop();

		bool IsPlaying();
		void SetTicksPerFrame(float ticksPerFrame);
		float GetTicksPerFrame();
		std::string GetData();
		void SetAnimationName(std::string animationName);
		std::string GetAnimationName();

		// Test Animations
		void LerpToCenter();
		void SetPlayAnimation(std::function<void(std::shared_ptr<GameObject>)> callback);

		std::function<void(std::shared_ptr<GameObject>)> PlayAnimationFunction;
	private:
		std::string animationName;
		bool _playing;
		float ticksPerFrame;
		int animationStartTime;
	};
}

