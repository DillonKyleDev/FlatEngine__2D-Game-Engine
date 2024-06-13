#include "Animation.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Vector2.h"
#include <cmath>


namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		SetType(ComponentTypes::Animation);
		SetID(myID);
		SetParentID(parentID);
		animationName = "";
		animationProperties = {};
		animationPath = "";
		_playing = false;
		animationStartTime = -1;
	}

	Animation::Animation(std::shared_ptr<Animation> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Animation);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		animationName = toCopy->GetAnimationName();
		animationProperties = toCopy->animationProperties;
		animationPath = toCopy->GetAnimationPath();
		_playing = false;
		animationStartTime = toCopy->animationStartTime;
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
		_playing = true;
		// Start animation timer
		animationStartTime = startTime;
	}

	void Animation::Stop()
	{
		_playing = false;
	}

	bool Animation::IsPlaying()
	{
		return _playing;
	}

	std::string Animation::GetData()
	{
		json jsonData = {
			{ "type", "Animation" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "path", animationPath }
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
		std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(GetParentID());
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(ComponentTypes::Transform));

		// Save original position
		static Vector2 startingPoint = transform->GetPosition();

		// If the animation should still be on the first frame
		//if (animationStartTime + ticksPerFrame * 1 < FlatEngine::GetEllapsedGameTime())
		//{
		//	// Do first frame things
		//	transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), Vector2(0, 0), .01f));
		//}

		// For 500 ticks, do this:
		if (animationStartTime + 500 > FlatEngine::GetEllapsedGameTimeInMs())
		{
			// Do first frame things
			transform->SetScale(FlatEngine::Lerp(transform->GetScale(), Vector2(2, 2), 0.1f));
		}
		else if (animationStartTime + 1000 > FlatEngine::GetEllapsedGameTimeInMs())
		{
			transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), Vector2(startingPoint.x + 16, startingPoint.y), 0.5f));
		}
		else if (animationStartTime + 1300 > FlatEngine::GetEllapsedGameTimeInMs())
		{
			transform->SetPosition(FlatEngine::Lerp(transform->GetPosition(), startingPoint, 0.1f));
		}
		else if (animationStartTime + 2000 > FlatEngine::GetEllapsedGameTimeInMs())
		{
			transform->SetScale(FlatEngine::Lerp(transform->GetScale(), Vector2(1, 1), 0.1f));
		}
		else
			Stop();
	}

	void Animation::PlayAnimation(int ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = animationProperties;

		if (!props->_isSorted)
			props->SortKeyFrames();

		static float lastTransformAnimationFrameEnd = 0;
		static float lastSpriteAnimationFrameEnd = 0;
		static float currentKeyFrame = (float)animationStartTime;
		static Vector2 lastFrameTransform = Vector2(0, 0);

		// While the animation is not over
		if (props->animationLength > ellapsedTime - animationStartTime)
		{
			// Transform Animation Frames
			for (std::vector<std::shared_ptr<S_Transform>>::iterator transformFrame = props->transformProperties.begin(); transformFrame != props->transformProperties.end();)
			{ 
				if (ellapsedTime < animationStartTime + (*transformFrame)->time)
				{
					std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = transformFrame;
					if (transformFrame - 1 >= props->transformProperties.begin())
						lastFrame = transformFrame - 1;

					int timeLeft = (int)(*transformFrame)->time - ellapsedTime - animationStartTime;
					float percentDone = (ellapsedTime - animationStartTime - (*lastFrame)->time) / ((*transformFrame)->time - (*lastFrame)->time);
					lastFrameTransform = Vector2((*lastFrame)->xMove, (*lastFrame)->yMove);
					std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();

					switch ((*transformFrame)->transformInterpType)
					{
						case Lerp:
						{
							float correctedX = (lastFrameTransform.x + ((*transformFrame)->xMove - lastFrameTransform.x) * percentDone);
							float correctedY = (lastFrameTransform.y + ((*transformFrame)->yMove - lastFrameTransform.y) * percentDone);

							transform->SetPosition(Vector2(correctedX, correctedY));
							break;
						}
						case Slerp:
						{
							float slerpYValue = ((percentDone) * 2) - 1;
							float slerpedPercentDone;

							if (percentDone <= .50f)
							{
								slerpedPercentDone = sqrt(1 - slerpYValue * slerpYValue) / 2;
							}
							else
							{
								slerpedPercentDone = 1 - (sqrt(1 - slerpYValue * slerpYValue) / 2);
							}

							float correctedX = (lastFrameTransform.x + ((*transformFrame)->xMove - lastFrameTransform.x) * slerpedPercentDone);
							float correctedY = (lastFrameTransform.y + ((*transformFrame)->yMove - lastFrameTransform.y) * slerpedPercentDone);

							transform->SetPosition(Vector2(correctedX, correctedY));
							break;
						}
					}
					break;
				}
				transformFrame = transformFrame + 1;
			}
			// Sprite Animation Frames
			for (const std::shared_ptr<S_Sprite>& spriteFrame : props->spriteProperties)
			{
				if (ellapsedTime < animationStartTime + spriteFrame->time)
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
		else if (props->_loop)
		{
			animationStartTime = ellapsedTime;
		}
		else
			Stop();
	}
}