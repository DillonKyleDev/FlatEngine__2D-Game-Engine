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
				static Vector2 lastFramePosition = Vector2(0, 0);
				static Vector2 lastFrameScale = Vector2(0, 0);
				float keyframeTime = (*frame)->time;
				Transform* transform = GetParent()->GetTransform();
				std::shared_ptr<S_Transform> thisFrameProps = (*frame);
				std::vector<std::shared_ptr<S_Transform>>::iterator lastFrame = frame;
				if (transformFrameCounter > 0)
				{
					lastFrame--;
				}

				if (keyframeTime == 0 && !(*frame)->_fired)
				{
					transform->SetPosition(Vector2(thisFrameProps->xPos, thisFrameProps->yPos));
					transform->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
					(*frame)->_fired = true;
				}
				else if ((ellapsedTime > (*lastFrame)->time + m_animationStartTime) && (ellapsedTime < m_animationStartTime + keyframeTime))
				{
					std::shared_ptr<S_Transform> lastFrameProps = (*lastFrame);
					float timeLeft = (m_animationStartTime + keyframeTime) - ellapsedTime;
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
					default:
						break;
					}
				}	
				transformFrameCounter++;
			}

			// Sprite Animation Frames
			if (GetParent()->GetSprite() != nullptr)
			{
				static Vector4 lastFrameSpriteTint = Vector4(1, 1, 1, 1);
				static Vector2 lastFrameOffset = Vector2(0, 0);
				static Vector2 lastFrameScale = Vector2(1, 1);

				int spriteFrameCounter = 0;
				for (std::vector<std::shared_ptr<S_Sprite>>::iterator frame = props->spriteProps.begin(); frame != props->spriteProps.end(); frame++)
				{
					float keyframeTime = (*frame)->time;
					Sprite* sprite = GetParent()->GetSprite();
					std::shared_ptr<S_Sprite> thisFrameProps = (*frame);
					std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = frame;
					if (spriteFrameCounter > 0)
					{
						lastFrame--;
					}

					if (keyframeTime == 0 && !(*frame)->_fired)
					{
						sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
						sprite->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
						sprite->SetTexture(thisFrameProps->path);
						sprite->SetTintColor(thisFrameProps->tintColor);
						(*frame)->_fired = true;
					}
					else if ((ellapsedTime > (*lastFrame)->time + m_animationStartTime) && (ellapsedTime < m_animationStartTime + keyframeTime))
					{
						std::shared_ptr<S_Sprite> lastFrameProps = (*lastFrame);

						float timeLeft = (m_animationStartTime + keyframeTime) - ellapsedTime;
						float percentDone = (float)(ellapsedTime - m_animationStartTime - lastFrameProps->time) / (keyframeTime - lastFrameProps->time);
						lastFrameOffset = Vector2(lastFrameProps->xOffset, lastFrameProps->yOffset);
						lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);


						switch (thisFrameProps->interpType)
						{
						case I_Lerp:
						{
							Vector4 correctedTintColor = thisFrameProps->tintColor;
							lastFrameSpriteTint = (*lastFrame)->tintColor;

							if (!thisFrameProps->_instantTintChange)
							{
								correctedTintColor = Vector4(lastFrameSpriteTint.x + (thisFrameProps->tintColor.x - lastFrameSpriteTint.x) * percentDone,
									lastFrameSpriteTint.y + (thisFrameProps->tintColor.y - lastFrameSpriteTint.y) * percentDone,
									lastFrameSpriteTint.z + (thisFrameProps->tintColor.z - lastFrameSpriteTint.z) * percentDone,
									lastFrameSpriteTint.w + (thisFrameProps->tintColor.w - lastFrameSpriteTint.w) * percentDone);
							}

							float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
							float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);

							if (thisFrameProps->path != "")
							{
								sprite->SetTexture(thisFrameProps->path);
							}
							if (correctedXScale != 0 && correctedYScale != 0)
							{
								sprite->SetScale(Vector2(correctedXScale, correctedYScale));
							}
							sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
							sprite->SetTintColor(correctedTintColor);
							break;
						}
						}
					}
					spriteFrameCounter++;
				}
			}


			//for (std::vector<std::shared_ptr<S_Sprite>>::iterator spriteFrame = props->spriteProps.begin(); spriteFrame != props->spriteProps.end(); spriteFrame++)
			//{
			//	if (ellapsedTime < m_animationStartTime + (*spriteFrame)->time)
			//	{
			//		FlatEngine::Sprite* sprite = GetParent()->GetSprite();
			//		std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = spriteFrame;
			//		if (lastFrame != props->spriteProps.begin() && lastFrame -1 >= props->spriteProps.begin())
			//			lastFrame = lastFrame - 1;

			//		float timeLeft = (*spriteFrame)->time - ellapsedTime - m_animationStartTime;
			//		float percentDone = (ellapsedTime - m_animationStartTime - (*lastFrame)->time) / ((*spriteFrame)->time - (*lastFrame)->time);
			//		
			//		Vector4 correctedTintColor = (*spriteFrame)->tintColor;

			//		if (!(*spriteFrame)->_instantTintChange)
			//		{
			//			lastFrameSpriteTint = (*lastFrame)->tintColor;
			//			correctedTintColor = Vector4(lastFrameSpriteTint.x + ((*spriteFrame)->tintColor.x - lastFrameSpriteTint.x) * percentDone,
			//				lastFrameSpriteTint.y + ((*spriteFrame)->tintColor.y - lastFrameSpriteTint.y) * percentDone,
			//				lastFrameSpriteTint.z + ((*spriteFrame)->tintColor.z - lastFrameSpriteTint.z) * percentDone,
			//				lastFrameSpriteTint.w + ((*spriteFrame)->tintColor.w - lastFrameSpriteTint.w) * percentDone);
			//		}

			//		if ((*spriteFrame)->path != "")
			//			sprite->SetTexture((*spriteFrame)->path);
			//		Vector2 spriteOffset = sprite->GetOffset();
			//		sprite->SetOffset(Vector2((*spriteFrame)->xOffset, (*spriteFrame)->yOffset));
			//		sprite->SetTintColor(correctedTintColor);
			//		break;
			//	}		
			//	spriteFrameCounter++;
			//}
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