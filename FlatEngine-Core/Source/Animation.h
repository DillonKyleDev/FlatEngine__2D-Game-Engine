#pragma once
#include "Component.h"
#include <map>
#include "Vector4.h"


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
			Slerp
		};

		// Property structs

		struct S_Property {
			float time = 0;
			std::string name;
			bool _fired;
		};
		struct S_Transform : public S_Property {
			InterpType transformInterpType = Lerp;
			float transformSpeed = 0.1f;
			InterpType scaleInterpType = Lerp;
			float scaleSpeed = 0.1f;
			float xMove = 0;
			float yMove = 0;
			float xScale = 0;
			float yScale = 0;
		};
		struct S_Sprite : public S_Property {
			InterpType interpType = Lerp;
			float speed = 0.1f;
			std::string path = "";
			float xOffset = 0;
			float yOffset = 0;
			Vector4 tintColor = Vector4(1, 1, 1, 1);
			bool _instantTintChange = false;
		};
		struct S_Camera : public S_Property {
			bool _isPrimaryCamera = false;
		};
		struct S_Script : public S_Property {
			std::string path = "";
		};
		struct S_Button : public S_Property {
			bool _isActive = true;
		};
		struct S_Canvas : public S_Property {
		};
		struct S_Audio : public S_Property {
			std::string path = "";
			bool _isMusic = false;
		};
		struct S_Text : public S_Property {
			std::string path = "";
			std::string text = "";
			std::string color = "";
		};
		struct S_BoxCollider : public S_Property {
			bool _isActive = true;
		};
		struct S_CircleCollider : public S_Property {
			bool _isActive = true;
		};
		struct S_RigidBody : public S_Property {
			InterpType interpType = Lerp;
			float speed = 0.1f;
			bool _isActive = true;
			float gravityScale = 1;
		};
		struct S_CharacterController : public S_Property {
			bool _isActive = true;
		};
		struct S_Event : public S_Property {
			std::string functionName = "";
		};

		struct S_AnimationProperties {
			std::string animationName = "";
			std::string animationPath = "";
			float animationLength = 0.0f;
			bool _isSorted = false;
			bool _loop = false;
			std::vector<std::shared_ptr<S_Transform>> transformProperties = std::vector<std::shared_ptr<S_Transform>>();
			std::vector<std::shared_ptr<S_Sprite>> spriteProperties = std::vector<std::shared_ptr<S_Sprite>>();
			std::vector<std::shared_ptr<S_Camera>> cameraProperties = std::vector<std::shared_ptr<S_Camera>>();
			std::vector<std::shared_ptr<S_Script>> scriptProperties = std::vector<std::shared_ptr<S_Script>>();
			std::vector<std::shared_ptr<S_Button>> buttonProperties = std::vector<std::shared_ptr<S_Button>>();
			std::vector<std::shared_ptr<S_Canvas>> canvasProperties = std::vector<std::shared_ptr<S_Canvas>>();
			std::vector<std::shared_ptr<S_Audio>> audioProperties = std::vector<std::shared_ptr<S_Audio>>();
			std::vector<std::shared_ptr<S_Text>> textProperties = std::vector<std::shared_ptr<S_Text>>();
			std::vector<std::shared_ptr<S_BoxCollider>> boxColliderProperties = std::vector<std::shared_ptr<S_BoxCollider>>();
			std::vector<std::shared_ptr<S_CircleCollider>> circleColliderProperties = std::vector<std::shared_ptr<S_CircleCollider>>();
			std::vector<std::shared_ptr<S_RigidBody>> rigidBodyProperties = std::vector<std::shared_ptr<S_RigidBody>>();
			std::vector<std::shared_ptr<S_CharacterController>> characterControllerProperties = std::vector<std::shared_ptr<S_CharacterController>>();
			std::vector<std::shared_ptr<S_Event>> eventProperties = std::vector< std::shared_ptr<S_Event>>();

			void SortKeyFrames() 
			{
				float lastKeyFrameEndTime = 0.0f;

				bool _didntSwapTransform = false;
				while (!_didntSwapTransform)
				{
					_didntSwapTransform = true;
					for (std::vector<std::shared_ptr<S_Transform>>::iterator keyFrame = transformProperties.begin(); keyFrame != transformProperties.end(); keyFrame++)
					{
						std::vector<std::shared_ptr<S_Transform>>::iterator keyFrame2 = keyFrame + 1;
						if (keyFrame2 != transformProperties.end() && (*keyFrame)->time > (*keyFrame2)->time)
						{
							std::swap(*keyFrame, *keyFrame2);
							_didntSwapTransform = false;
						}
					}
				}
				if (transformProperties.size() > 0)
					lastKeyFrameEndTime = transformProperties.back()->time;

				bool _didntSwapSprite = false;
				while (!_didntSwapSprite)
				{
					_didntSwapSprite = true;
					for (std::vector<std::shared_ptr<S_Sprite>>::iterator keyFrame = spriteProperties.begin(); keyFrame != spriteProperties.end(); keyFrame++)
					{
						std::vector<std::shared_ptr<S_Sprite>>::iterator keyFrame2 = keyFrame + 1;
						if (keyFrame2 != spriteProperties.end() && (*keyFrame)->time > (*keyFrame2)->time)
						{
							std::swap(*keyFrame, *keyFrame2);
							_didntSwapSprite = false;
						}
					}
				}
				if (spriteProperties.size() > 0 && spriteProperties.back()->time > lastKeyFrameEndTime)
				{
					lastKeyFrameEndTime = spriteProperties.back()->time;
				}

				bool _didntSwapEvent = false;
				while (!_didntSwapEvent)
				{
					_didntSwapEvent = true;
					for (std::vector<std::shared_ptr<S_Event>>::iterator keyFrame = eventProperties.begin(); keyFrame != eventProperties.end(); keyFrame++)
					{
						std::vector<std::shared_ptr<S_Event>>::iterator keyFrame2 = keyFrame + 1;
						if (keyFrame2 != eventProperties.end() && (*keyFrame)->time > (*keyFrame2)->time)
						{
							std::swap(*keyFrame, *keyFrame2);
							_didntSwapEvent = false;
						}
					}
				}
				if (eventProperties.size() > 0 && eventProperties.back()->time > lastKeyFrameEndTime)
					lastKeyFrameEndTime = eventProperties.back()->time;
				
				animationLength = lastKeyFrameEndTime;

				_isSorted = true;
			};
		};

		// Inherit from this struct to create new event property structs with useful event information to pass
		// to calling event function.
		struct S_EventProperties {
			// Event properties you might need to pass to calling event function
		};

		Animation(long myID = -1, long parentID = -1);
		Animation(Animation* toCopy, long newParentID, long myID = -1);
		~Animation();

		void AddFrame();
		void Play(long startTime = -1);
		void Stop();
		
		void PlayAnimation(long ellapsedTime);
		bool IsPlaying();
		std::string GetData();
		void SetAnimationName(std::string animationName);
		std::string GetAnimationName();
		void SetAnimationPath(std::string animationPath);
		std::string GetAnimationPath();
		void AddEventFunction(std::string name, std::function<void(GameObject)> callback);
		std::map<std::string, std::function<void(GameObject)>> GetEventFunctions();

		// Test Animations
		void LerpToCenter();
	
	private:
		std::shared_ptr<S_AnimationProperties> animationProperties;
		std::string animationName;
		std::string animationPath;
		bool _playing;
		long animationStartTime;
		std::map<std::string, std::function<void(GameObject)>> eventFunctions;
	};
}

