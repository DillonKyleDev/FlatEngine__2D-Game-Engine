#pragma once
#include "Component.h"
#include <map>


namespace FlatEngine
{
	class GameObject;

	class Animation : public Component
	{
	public:
		enum Properties {
			Transform,
			Sprite,
			Camera,
			Script,
			Button,
			Canvas,
			Audio,
			Text,
			BoxCollider,
			CircleCollider,
			RigidBody,
			CharacterController
		};

		// Property structs
		struct S_Transform {
			float time;
			float xMove;
			float yMove;
			float xScale;
			float yScale;
		};
		struct S_Sprite {
			float time;
			std::string path;
			float xOffset;
			float yOffset;
		};

		struct S_AnimationProperties {
			std::string animationName = "";
			std::string animationPath = "";
			std::vector<S_Transform> transformProperties = std::vector<S_Transform>();
			std::vector<S_Sprite> spriteProperties = std::vector<S_Sprite>();
		};

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
		S_AnimationProperties animationProperties;
		std::string animationName;
		bool _playing;
		float ticksPerFrame;
		int animationStartTime;
	};
}

