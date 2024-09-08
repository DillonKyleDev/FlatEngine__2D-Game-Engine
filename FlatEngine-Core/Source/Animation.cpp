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

	void Animation::PlayFromLua()
	{	
		Play(-1);
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

	void Animation::PlayAnimation(long ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = m_animationProperties;

		if (!props->b_isSorted)
			props->SortFrames();

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
			for (const std::shared_ptr<S_Event>& eventFrame : props->eventProps)
			{
				if ((eventFrame->time == 0 && !eventFrame->_fired) || (!eventFrame->_fired && (ellapsedTime >= m_animationStartTime + eventFrame->time || eventFrame->time == 0)))
				{
					CallLuaAnimationEventFunction(GetParent(), eventFrame->functionName);
					eventFrame->_fired = true;
				}
			}

			// Transform Animation Frames
			int transformFrameCounter = 0;
			for (std::vector<std::shared_ptr<S_Transform>>::iterator frame = props->transformProps.begin(); frame != props->transformProps.end(); frame++)
			{ 
				float keyframeTime = (*frame)->time;
				Transform* transform = GetParent()->GetTransform();
				std::shared_ptr<S_Transform> thisFrameProps = (*frame);

				if (transformFrameCounter == 0)
				{
					transform->SetPosition(Vector2(thisFrameProps->xPos, thisFrameProps->yPos));
					transform->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
				}
				else if (ellapsedTime < m_animationStartTime + keyframeTime)
				{
					std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = frame;
					if (transformFrameCounter > 0)
					{
						lastFrame--;
					}

					std::shared_ptr<S_Transform> lastFrameProps = (*lastFrame);

					float timeLeft = keyframeTime - ellapsedTime - m_animationStartTime;
					float percentDone = (float)(ellapsedTime - m_animationStartTime - lastFrameProps->time) / (keyframeTime - lastFrameProps->time);
					lastFramePosition = Vector2(lastFrameProps->xPos, lastFrameProps->yPos);
					lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);


					switch (thisFrameProps->transformInterpType)
					{
					case I_Lerp:
					{
						float correctedX = (lastFramePosition.x + (thisFrameProps->xPos - lastFramePosition.x) * percentDone);
						float correctedY = (lastFramePosition.y + (thisFrameProps->yPos - lastFramePosition.y) * percentDone);
						float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
						float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);

						transform->SetPosition(Vector2(correctedX, correctedY));
						if (correctedXScale != 0 && correctedYScale != 0)
							transform->SetScale(Vector2(correctedXScale, correctedYScale));
						break;
					}
					case I_Slerp:
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

						float correctedX = (lastFramePosition.x + (thisFrameProps->xPos - lastFramePosition.x) * slerpedPercentDone);
						float correctedY = (lastFramePosition.y + (thisFrameProps->yPos - lastFramePosition.y) * slerpedPercentDone);

						transform->SetPosition(Vector2(correctedX, correctedY));
						break;
					}
					}
				}	
				transformFrameCounter++;
			}
			// Sprite Animation Frames
			int spriteFrameCounter = 0;
			for (std::vector<std::shared_ptr<S_Sprite>>::iterator spriteFrame = props->spriteProps.begin(); spriteFrame != props->spriteProps.end(); spriteFrame++)
			{
				if (ellapsedTime < m_animationStartTime + (*spriteFrame)->time)
				{
					FlatEngine::Sprite* sprite = GetParent()->GetSprite();
					std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = spriteFrame;
					if (lastFrame != props->spriteProps.begin() && lastFrame -1 >= props->spriteProps.begin())
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
				spriteFrameCounter++;
			}
		}
		else if (props->_loop)
		{
			m_animationStartTime = ellapsedTime;
		}
		else
		{
			Stop();
		}
	}
}