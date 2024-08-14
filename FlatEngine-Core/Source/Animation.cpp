#include "Animation.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Vector2.h"
#include <cmath>


namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		SetType(ComponentTypes::T_Animation);
		SetID(myID);
		SetParentID(parentID);
		animationName = "";
		animationProperties = {};
		animationPath = "";
		_playing = false;
		animationStartTime = -1;
	}

	Animation::Animation(Animation* toCopy, long newParentID, long myID)
	{
		SetType(ComponentTypes::T_Animation);
		if (myID != -1)
			SetID(myID);
		else
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

	void Animation::Play(long startTime)
	{
		animationProperties = LoadAnimationFile(animationPath);
		_playing = true;
		// Start animation timer
		if (startTime != -1)
			animationStartTime = startTime;
		else
			animationStartTime = FlatEngine::GetEllapsedGameTimeInMs();
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
		animationProperties = LoadAnimationFile(path);
		if (path != "")
			animationName = GetFilenameFromPath(path);
	}

	std::string Animation::GetAnimationPath()
	{
		return animationPath;
	}

	void Animation::AddEventFunction(std::string name, std::function<void(GameObject)> callback)
	{
		std::pair <std::string, std::function<void(GameObject)>> functionPair = std::pair<std::string, std::function<void(GameObject)>>(name, callback);
		eventFunctions.emplace(functionPair);
	}

	std::map<std::string, std::function<void(GameObject)>> Animation::GetEventFunctions()
	{
		return eventFunctions;
	}


	void Animation::LerpToCenter()
	{
		// Get variables
		GameObject thisObject = FlatEngine::GetObjectById(GetParentID());
		FlatEngine::Transform* transform = thisObject.GetTransform();

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

	void Animation::PlayAnimation(long ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = animationProperties;

		if (!props->_isSorted)
			props->SortKeyFrames();

		static long lastTransformAnimationFrameEnd = 0;
		static long lastSpriteAnimationFrameEnd = 0;
		static long currentKeyFrame = animationStartTime;
		static Vector2 lastFramePosition = Vector2(0, 0);
		static Vector2 lastFrameScale = Vector2(0, 0);
		static Vector4 lastFrameSpriteTint = Vector4(1,1,1,1);

		// While the animation is not over
		if (props->animationLength > ellapsedTime - animationStartTime)
		{
			// Event Animation Frames
			for (const std::shared_ptr<S_Event>& eventFrame : props->eventProperties)
			{
				if ((eventFrame->time == 0 && !eventFrame->_fired) || (!eventFrame->_fired && (ellapsedTime >= animationStartTime + eventFrame->time || eventFrame->time == 0)))
				{
					for (std::pair<std::string, std::function<void(GameObject)>> eventFunction : eventFunctions)
					{
						if (eventFunction.first == eventFrame->functionName)
						{
							eventFunction.second(GetParent());
							eventFrame->_fired = true;
						}
					}
				}
			}
			// Transform Animation Frames
			for (std::vector<std::shared_ptr<S_Transform>>::iterator transformFrame = props->transformProperties.begin(); transformFrame != props->transformProperties.end();)
			{ 
				if (ellapsedTime < animationStartTime + (*transformFrame)->time)
				{
					std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = transformFrame;
					if (transformFrame - 1 >= props->transformProperties.begin())
						lastFrame = transformFrame - 1;

					float timeLeft = (*transformFrame)->time - ellapsedTime - animationStartTime;
					float percentDone = (float)(ellapsedTime - animationStartTime - (*lastFrame)->time) / (float)((*transformFrame)->time - (*lastFrame)->time);
					lastFramePosition = Vector2((*lastFrame)->xMove, (*lastFrame)->yMove);
					lastFrameScale = Vector2((*lastFrame)->xScale, (*lastFrame)->yScale);
					FlatEngine::Transform* transform = GetParent()->GetTransform();
					LogFloat(percentDone, "Percent Done: ");
					LogVector2(lastFrameScale, "lastFrameScale: ");
					//LogFloat(lastFrameScale, "lastFrameScale: ");
					switch ((*transformFrame)->transformInterpType)
					{
						case Lerp:
						{
							float correctedX = (lastFramePosition.x + ((*transformFrame)->xMove - lastFramePosition.x) * percentDone);
							float correctedY = (lastFramePosition.y + ((*transformFrame)->yMove - lastFramePosition.y) * percentDone);
							float correctedXScale = (lastFrameScale.x + ((*transformFrame)->xScale - lastFrameScale.x) * percentDone);
							float correctedYScale = (lastFrameScale.y + ((*transformFrame)->yScale - lastFrameScale.y) * percentDone);

							transform->SetPosition(Vector2(correctedX, correctedY));
							if (correctedXScale != 0 && correctedYScale != 0)
								transform->SetScale(Vector2(correctedXScale, correctedYScale));
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

							float correctedX = (lastFramePosition.x + ((*transformFrame)->xMove - lastFramePosition.x) * slerpedPercentDone);
							float correctedY = (lastFramePosition.y + ((*transformFrame)->yMove - lastFramePosition.y) * slerpedPercentDone);

							transform->SetPosition(Vector2(correctedX, correctedY));
							break;
						}
					}
					break;
				}
				transformFrame = transformFrame + 1;
			}
			// Sprite Animation Frames
			for (std::vector<std::shared_ptr<S_Sprite>>::iterator spriteFrame = props->spriteProperties.begin(); spriteFrame != props->spriteProperties.end();)
			{
				if (ellapsedTime < animationStartTime + (*spriteFrame)->time)
				{
					FlatEngine::Sprite* sprite = GetParent()->GetSprite();
					std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = spriteFrame;
					if (lastFrame != props->spriteProperties.begin() && lastFrame -1 >= props->spriteProperties.begin())
						lastFrame = lastFrame - 1;

					float timeLeft = (*spriteFrame)->time - ellapsedTime - animationStartTime;
					float percentDone = (ellapsedTime - animationStartTime - (*lastFrame)->time) / ((*spriteFrame)->time - (*lastFrame)->time);
					
					Vector4 correctedTintColor = (*spriteFrame)->tintColor;

					if (!(*spriteFrame)->_instantTintChange)
					{
						lastFrameSpriteTint = (*lastFrame)->tintColor;
						correctedTintColor = Vector4(lastFrameSpriteTint.x + ((*spriteFrame)->tintColor.x - lastFrameSpriteTint.x) * percentDone,
							lastFrameSpriteTint.y + ((*spriteFrame)->tintColor.y - lastFrameSpriteTint.y) * percentDone,
							lastFrameSpriteTint.z + ((*spriteFrame)->tintColor.z - lastFrameSpriteTint.z) * percentDone,
							lastFrameSpriteTint.w + ((*spriteFrame)->tintColor.w - lastFrameSpriteTint.w) * percentDone);
					}

					if ((*spriteFrame)->path != "")
						sprite->SetTexture((*spriteFrame)->path);
					Vector2 spriteOffset = sprite->GetOffset();
					sprite->SetOffset(Vector2((*spriteFrame)->xOffset, (*spriteFrame)->yOffset));
					sprite->SetTintColor(correctedTintColor);
					break;
				}

				spriteFrame++;
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