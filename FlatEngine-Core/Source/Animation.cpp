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
		m_animationName = "";
		m_animationProperties = {};
		m_animationPath = "";
		m_b_playing = false;
		m_animationStartTime = -1;
	}

	Animation::~Animation()
	{
	}

	void Animation::AddFrame()
	{
	}

	void Animation::Play(long startTime)
	{
		m_animationProperties = LoadAnimationFile(m_animationPath);
		m_b_playing = true;
		// Start animation timer
		if (startTime != -1)
			m_animationStartTime = startTime;
		else
			m_animationStartTime = FlatEngine::GetEllapsedGameTimeInMs();
	}

	void Animation::Stop()
	{
		m_b_playing = false;
	}

	bool Animation::IsPlaying()
	{
		return m_b_playing;
	}

	std::string Animation::GetData()
	{
		json jsonData = {
			{ "type", "Animation" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "path", m_animationPath }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Animation::SetAnimationName(std::string name)
	{
		m_animationName = name;
	}

	std::string Animation::GetAnimationName()
	{
		return m_animationName;
	}

	void Animation::SetAnimationPath(std::string path)
	{
		m_animationPath = path;
		m_animationProperties = LoadAnimationFile(path);
		if (path != "")
			m_animationName = GetFilenameFromPath(path);
	}

	std::string Animation::GetAnimationPath()
	{
		return m_animationPath;
	}

	void Animation::AddEventFunction(std::string name, std::function<void(GameObject*)> callback)
	{
		std::pair <std::string, std::function<void(GameObject*)>> functionPair = std::pair<std::string, std::function<void(GameObject*)>>(name, callback);
		m_eventFunctions.emplace(functionPair);
	}

	std::map<std::string, std::function<void(GameObject*)>> Animation::GetEventFunctions()
	{
		return m_eventFunctions;
	}

	void Animation::PlayAnimation(long ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = m_animationProperties;

		if (!props->_isSorted)
			props->SortKeyFrames();

		static long lastTransformAnimationFrameEnd = 0;
		static long lastSpriteAnimationFrameEnd = 0;
		static long currentKeyFrame = m_animationStartTime;
		static Vector2 lastFramePosition = Vector2(0, 0);
		static Vector2 lastFrameScale = Vector2(0, 0);
		static Vector4 lastFrameSpriteTint = Vector4(1,1,1,1);

		// While the animation is not over
		if (props->animationLength > ellapsedTime - m_animationStartTime)
		{
			// Event Animation Frames
			for (const std::shared_ptr<S_Event>& eventFrame : props->eventProperties)
			{
				if ((eventFrame->time == 0 && !eventFrame->_fired) || (!eventFrame->_fired && (ellapsedTime >= m_animationStartTime + eventFrame->time || eventFrame->time == 0)))
				{
					for (std::pair<std::string, std::function<void(GameObject*)>> eventFunction : m_eventFunctions)
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
				if (ellapsedTime < m_animationStartTime + (*transformFrame)->time)
				{
					std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = transformFrame;
					if (transformFrame - 1 >= props->transformProperties.begin())
						lastFrame = transformFrame - 1;

					float timeLeft = (*transformFrame)->time - ellapsedTime - m_animationStartTime;
					float percentDone = (float)(ellapsedTime - m_animationStartTime - (*lastFrame)->time) / (float)((*transformFrame)->time - (*lastFrame)->time);
					lastFramePosition = Vector2((*lastFrame)->xMove, (*lastFrame)->yMove);
					lastFrameScale = Vector2((*lastFrame)->xScale, (*lastFrame)->yScale);
					FlatEngine::Transform* transform = GetParent()->GetTransform();

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
				if (ellapsedTime < m_animationStartTime + (*spriteFrame)->time)
				{
					FlatEngine::Sprite* sprite = GetParent()->GetSprite();
					std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = spriteFrame;
					if (lastFrame != props->spriteProperties.begin() && lastFrame -1 >= props->spriteProperties.begin())
						lastFrame = lastFrame - 1;

					float timeLeft = (*spriteFrame)->time - ellapsedTime - m_animationStartTime;
					float percentDone = (ellapsedTime - m_animationStartTime - (*lastFrame)->time) / ((*spriteFrame)->time - (*lastFrame)->time);
					
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
			m_animationStartTime = ellapsedTime;
		}
		else
			Stop();
	}
}