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

		enum InterpType {
			Lerp,
		};

		// Property structs
		struct S_Transform {
			InterpType transformInterpType = Lerp;
			float transformSpeed = 0.1f;
			InterpType scaleInterpType = Lerp;
			float scaleSpeed = 0.1f;
			float time = 0;
			float xMove = 0;
			float yMove = 0;
			float xScale = 0;
			float yScale = 0;
		};
		struct S_Sprite {
			InterpType interpType = Lerp;
			float speed = 0.1f;
			float time = 0;
			std::string path = "";
			float xOffset = 0;
			float yOffset = 0;
		};
		struct S_Camera {
			float time = 0;
			bool _isPrimaryCamera = false;
		};
		struct S_Script {
			float time = 0;
			std::string path = "";
		};
		struct S_Button {
			float time = 0;
			bool _isActive = true;
		};
		struct S_Canvas {
			float time = 0;
		};
		struct S_Audio {
			float time = 0;
			std::string path = "";
			bool _isMusic = false;
		};
		struct S_Text {
			float time = 0;
			std::string path = "";
			std::string text = "";
			std::string color = "";
		};
		struct S_BoxCollider {
			float time = 0;
			bool _isActive = true;
		};
		struct S_CircleCollider {
			float time = 0;
			bool _isActive = true;
		};
		struct S_RigidBody {
			InterpType interpType = Lerp;
			float speed = 0.1f;
			float time = 0;
			bool _isActive = true;
			float gravityScale = 1;
		};
		struct S_CharacterController {
			float time = 0;
			bool _isActive = true;
		};

		struct S_AnimationProperties {
			std::string animationName = "";
			std::string animationPath = "";
			float animationLength = 0;
			std::vector<S_Transform> transformProperties = std::vector<S_Transform>();
			std::vector<S_Sprite> spriteProperties = std::vector<S_Sprite>();
			std::vector<S_Camera> cameraProperties = std::vector<S_Camera>();
			std::vector<S_Script> scriptProperties = std::vector<S_Script>();
			std::vector<S_Button> buttonProperties = std::vector<S_Button>();
			std::vector<S_Canvas> canvasProperties = std::vector<S_Canvas>();
			std::vector<S_Audio> audioProperties = std::vector<S_Audio>();
			std::vector<S_Text> textProperties = std::vector<S_Text>();
			std::vector<S_BoxCollider> boxColliderProperties = std::vector<S_BoxCollider>();
			std::vector<S_CircleCollider> circleColliderProperties = std::vector<S_CircleCollider>();
			std::vector<S_RigidBody> rigidBodyProperties = std::vector<S_RigidBody>();
			std::vector<S_CharacterController> characterControllerProperties = std::vector<S_CharacterController>();
		};

		Animation(long myID = -1, long parentID = -1);
		Animation(std::shared_ptr<Animation> toCopy);
		~Animation();

		void AddFrame();
		void Play(int startTime);
		void Stop();

		bool IsPlaying();
		void SetTicksPerFrame(float ticksPerFrame);
		float GetTicksPerFrame();
		std::string GetData();
		void SetAnimationName(std::string animationName);
		std::string GetAnimationName();
		void SetAnimationPath(std::string animationPath);
		std::string GetAnimationPath();

		// Test Animations
		void LerpToCenter();
		void PlayAnimation(int ellapsedTime);

	private:
		std::shared_ptr<S_AnimationProperties> animationProperties;
		std::string animationName;
		std::string animationPath;
		bool _playing;
		float ticksPerFrame;
		int animationStartTime;
	};
}

