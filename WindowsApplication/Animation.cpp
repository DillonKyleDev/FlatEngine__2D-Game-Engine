#include "Animation.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Vector2.h"


namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Animation);
		this->SetID(myID);
		this->SetParentID(parentID);
		animationName = "";
		animationProperties = {};
		animationPath = "";
		this->_playing = false;
		this->ticksPerFrame = 10;
		this->animationStartTime = -1;
	}

	Animation::Animation(std::shared_ptr<Animation> toCopy, long newParentID)
	{
		this->SetType(ComponentTypes::Animation);
		this->SetID(GetNextComponentID());
		this->SetParentID(newParentID);
		animationName = toCopy->GetAnimationName();
		animationProperties = toCopy->animationProperties;
		animationPath = toCopy->GetAnimationPath();
		this->_playing = false;
		this->ticksPerFrame = toCopy->GetTicksPerFrame();
		this->animationStartTime = toCopy->animationStartTime;
	}

	Animation::~Animation()
	{
	}

	void Animation::AddFrame()
	{

	}

	void Animation::Play(int startTime)
	{
		animationProperties = FlatEngine::FlatGui::LoadAnimationFile(animationPath);
		this->_playing = true;
		// Start animation timer
		this->animationStartTime = startTime;
	}

	void Animation::Stop()
	{
		this->_playing = false;
	}

	bool Animation::IsPlaying()
	{
		return this->_playing;
	}

	void Animation::SetTicksPerFrame(float ticksPerFrame)
	{
		this->ticksPerFrame = ticksPerFrame;
	}

	float Animation::GetTicksPerFrame()
	{
		return this->ticksPerFrame;
	}

	std::string Animation::GetData()
	{
		json jsonData = {
			{ "type", "Animation" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "path", this->animationPath }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Animation::SetAnimationName(std::string name)
	{
		animationName = name;
	}

	std::string Animation::GetAnimationName()
	{
		return animationName;
	}

	void Animation::SetAnimationPath(std::string path)
	{
		animationPath = path;
		animationProperties = FlatEngine::FlatGui::LoadAnimationFile(path);
	}

	std::string Animation::GetAnimationPath()
	{
		return animationPath;
	}


	void Animation::LerpToCenter()
	{
		// Get variables
		std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(this->GetParentID());
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(ComponentTypes::Transform));

		// Save original position
		static Vector2 startingPoint = transform->GetPosition();

		// If the animation should still be on the first frame
		//if (animationStartTime + this->ticksPerFrame * 1 < FlatEngine::GetEllapsedGameTime())
		//{
		//	// Do first frame things
		//	transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), Vector2(0, 0), .01f));
		//}

		// For 500 ticks, do this:
		if (animationStartTime + 500 > FlatEngine::GetEllapsedGameTime())
		{
			// Do first frame things
			transform->SetScale(FlatEngine::Lerp(transform->GetScale(), Vector2(2, 2), 0.1f));
		}
		else if (animationStartTime + 1000 > FlatEngine::GetEllapsedGameTime())
		{
			transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), Vector2(startingPoint.x + 16, startingPoint.y), 0.5f));
		}
		else if (animationStartTime + 1300 > FlatEngine::GetEllapsedGameTime())
		{
			transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), startingPoint, 0.1f));
		}
		else if (animationStartTime + 2000 > FlatEngine::GetEllapsedGameTime())
		{
			transform->SetScale(FlatEngine::Lerp(transform->GetScale(), Vector2(1, 1), 0.1f));
		}
		else
			this->Stop();
	}

	void Animation::PlayAnimation(int ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = animationProperties;

		if (!props->_isSorted)
		{
			props->SortKeyFrames();
			LogFloat(props->animationLength, "Animation Length: ");
		}

		static float lastTransformAnimationFrameEnd = 0;
		static float lastSpriteAnimationFrameEnd = 0;
		static float currentKeyFrame = animationStartTime;

		// While the animation is not over
		if (props->animationLength > ellapsedTime - animationStartTime)
		{
			// Transform Animation Frames
			for (std::vector<std::shared_ptr<S_Transform>>::iterator transformFrame = props->transformProperties.begin(); transformFrame != props->transformProperties.end();)
			{ 
				if (ellapsedTime < (*transformFrame)->time)
				{
					std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = transformFrame;
					if (transformFrame - 1 >= props->transformProperties.begin())
						lastFrame = transformFrame - 1;

					int timeLeft = (*transformFrame)->time - ellapsedTime - animationStartTime;
					float percentDone = (ellapsedTime - (*lastFrame)->time) / ((*transformFrame)->time - (*lastFrame)->time);

					// Save original position
					std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
					static Vector2 startingPoint = transform->GetPosition();
					static Vector2 startingScale = transform->GetScale();

					// Before signaling that we've changed keyframes, save the current position to be referenced by the current keyframe instead of the very first starting point
					if ((*transformFrame)->time > currentKeyFrame)
						startingPoint = transform->GetPosition();
					else if ((*transformFrame)->time < currentKeyFrame)
						startingPoint = Vector2(transform->GetPosition().x - (*transformFrame)->xMove, transform->GetPosition().y - (*transformFrame)->yMove);
					// Then signal the change to a new keyframe
					currentKeyFrame = (*transformFrame)->time;

					switch ((*transformFrame)->transformInterpType)
					{
						case Lerp:
						{
							float correctedX = (startingPoint.x + (*transformFrame)->xMove * percentDone);
							float correctedY = (startingPoint.y + (*transformFrame)->yMove * percentDone);

							transform->SetPosition(Vector2(correctedX, correctedY));
							break;
						}
					}
					//switch ((*transformFrame)->scaleInterpType)
					//{
					//	case Lerp:
					//	{
					//		transform->SetScale(FlatEngine::Lerp(1, Vector2(1 + (*transformFrame)->xScale, 1 + (*transformFrame)->yScale), (*transformFrame)->scaleSpeed));
					//		break;
					//	}
					//}
					break;
				}
				transformFrame = transformFrame + 1;
			}
			// Sprite Animation Frames
			for (const std::shared_ptr<S_Sprite>& spriteFrame : props->spriteProperties)
			{
				if (ellapsedTime < spriteFrame->time)
				{
					std::shared_ptr<FlatEngine::Sprite> sprite = GetParent()->GetSpriteComponent();

					if (spriteFrame->path != "")
						sprite->SetTexture(spriteFrame->path);

					Vector2 spriteOffset = sprite->GetOffset();
					sprite->SetOffset(Vector2(spriteOffset.x + spriteFrame->xOffset, spriteOffset.y + spriteFrame->yOffset));
					break;
				}
			}
		}
		else
			this->Stop();
	}
}