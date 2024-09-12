#include "Animation.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Text.h"
#include "Audio.h"
#include "Vector2.h"

#include <cmath>


namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		SetType(T_Animation);
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

	void Animation::Play(long startTime)
	{
		m_animationProperties = LoadAnimationFile(m_animationPath);
		m_b_playing = true;
		
		if (startTime != -1)
		{
			m_animationStartTime = startTime;
		}
		else
		{
			m_animationStartTime = FlatEngine::GetEllapsedGameTimeInMs();
		}
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
		{
			m_animationName = GetFilenameFromPath(path);
		}
	}

	std::string Animation::GetAnimationPath()
	{
		return m_animationPath;
	}

	void Animation::PlayAnimation(long ellapsedTime)
	{
		std::shared_ptr<S_AnimationProperties> props = m_animationProperties;

		if (!props->b_isSorted)
		{
			props->SortFrames();
		}
		
		// While the animation is not over (with a 1 second buffer to catch the last animation keyframes)
		if (props->animationLength + 1000 > ellapsedTime - m_animationStartTime)
		{
			// Event Animation Frames
			for (const std::shared_ptr<S_Event>& eventFrame : props->eventProps)
			{
				if ((eventFrame->time == 0 && !eventFrame->b_fired) || (!eventFrame->b_fired && (ellapsedTime >= m_animationStartTime + eventFrame->time || eventFrame->time == 0)))
				{
					CallLuaAnimationEventFunction(GetParent(), eventFrame->functionName);
					eventFrame->b_fired = true;
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
				bool b_posAnimated = thisFrameProps->b_posAnimated;
				bool b_scaleAnimated = thisFrameProps->b_scaleAnimated;

				float lastFrameTime = 0;
				if (transformFrameCounter > 0 && props->transformProps.size() > 1)
				{
					lastFrame--;
					lastFrameTime = (*lastFrame)->time;
				}	

				if (keyframeTime == 0 && !(*frame)->b_fired)
				{
					if (b_posAnimated)
					{
						transform->SetPosition(Vector2(thisFrameProps->xPos, thisFrameProps->yPos));
					}
					if (b_scaleAnimated)
					{
						transform->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
					}
					(*frame)->b_fired = true;
				}
				else if ((ellapsedTime > lastFrameTime + m_animationStartTime) && (ellapsedTime < m_animationStartTime + keyframeTime))
				{
					std::shared_ptr<S_Transform> lastFrameProps = (*lastFrame);
					float timeLeft = (m_animationStartTime + keyframeTime) - ellapsedTime;
					float percentDone = (float)(ellapsedTime - m_animationStartTime - lastFrameTime) / (keyframeTime - lastFrameTime);
					lastFramePosition = Vector2(lastFrameProps->xPos, lastFrameProps->yPos);
					lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);

					switch (thisFrameProps->transformInterpType)
					{
					case I_Lerp:
					{
						if (b_posAnimated)
						{
							float correctedX = (lastFramePosition.x + (thisFrameProps->xPos - lastFramePosition.x) * percentDone);
							float correctedY = (lastFramePosition.y + (thisFrameProps->yPos - lastFramePosition.y) * percentDone);
							transform->SetPosition(Vector2(correctedX, correctedY));
						}
						if (b_scaleAnimated)
						{
							float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
							float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);
							if (correctedXScale != 0 && correctedYScale != 0)
							{
								transform->SetScale(Vector2(correctedXScale, correctedYScale));
							}
						}
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
				Sprite* sprite = GetParent()->GetSprite();
				static Vector4 lastFrameSpriteTint = Vector4(1, 1, 1, 1);
				static Vector2 lastFrameOffset = Vector2(0, 0);
				static Vector2 lastFrameScale = Vector2(1, 1);

				int spriteFrameCounter = 0;
				for (std::vector<std::shared_ptr<S_Sprite>>::iterator frame = props->spriteProps.begin(); frame != props->spriteProps.end(); frame++)
				{
					float keyframeTime = (*frame)->time;
					std::shared_ptr<S_Sprite> thisFrameProps = (*frame);
					std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = frame;
					bool b_pathAnimated = thisFrameProps->b_pathAnimated;
					bool b_scaleAnimated = thisFrameProps->b_scaleAnimated;				
					bool b_offsetAnimated = thisFrameProps->b_offsetAnimated;
					bool b_tintColorAnimated = thisFrameProps->b_tintColorAnimated;

					float lastFrameTime = 0;
					if (spriteFrameCounter > 0 && props->spriteProps.size() > 1)
					{
						lastFrame--;
						lastFrameTime = (*lastFrame)->time;
					}

					if (keyframeTime == 0 && !(*frame)->b_fired)
					{
						if (b_offsetAnimated)
						{
							sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
						}
						if (b_scaleAnimated)
						{
							sprite->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
						}
						if (b_pathAnimated && !thisFrameProps->b_fired)
						{
							sprite->SetTexture(thisFrameProps->path);
						}
						if (b_tintColorAnimated)
						{
							sprite->SetTintColor(thisFrameProps->tintColor);
						}
						thisFrameProps->b_fired = true;
					}
					else if ((ellapsedTime > lastFrameTime + m_animationStartTime) && (ellapsedTime < m_animationStartTime + keyframeTime))
					{
						std::shared_ptr<S_Sprite> lastFrameProps = (*lastFrame);
						float timeLeft = (m_animationStartTime + keyframeTime) - ellapsedTime;
						float percentDone = (float)(ellapsedTime - m_animationStartTime - lastFrameTime) / (keyframeTime - lastFrameTime);

						if (b_pathAnimated && !thisFrameProps->b_fired && thisFrameProps->path != "")
						{
							sprite->SetTexture(thisFrameProps->path);
							thisFrameProps->b_fired = true;
						}

						switch (thisFrameProps->interpType)
						{
						case I_Lerp:
						{
							if (b_tintColorAnimated)
							{
								Vector4 correctedTintColor = thisFrameProps->tintColor;
								lastFrameSpriteTint = (*lastFrame)->tintColor;
								if (!thisFrameProps->b_instantTintChange)
								{
									correctedTintColor = Vector4(lastFrameSpriteTint.x + (thisFrameProps->tintColor.x - lastFrameSpriteTint.x) * percentDone,
										lastFrameSpriteTint.y + (thisFrameProps->tintColor.y - lastFrameSpriteTint.y) * percentDone,
										lastFrameSpriteTint.z + (thisFrameProps->tintColor.z - lastFrameSpriteTint.z) * percentDone,
										lastFrameSpriteTint.w + (thisFrameProps->tintColor.w - lastFrameSpriteTint.w) * percentDone);
								}
								sprite->SetTintColor(correctedTintColor);
							}
							if (b_scaleAnimated)
							{
								lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);
								float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
								float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);
								if (correctedXScale != 0 && correctedYScale != 0)
								{
									sprite->SetScale(Vector2(correctedXScale, correctedYScale));
								}
							}
							if (b_offsetAnimated)
							{
								lastFrameOffset = Vector2(lastFrameProps->xOffset, lastFrameProps->yOffset);
								sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
							}
							break;
						}
						default:
							break;
						}
					}
					spriteFrameCounter++;
				}
			}

			// Text Animation Frames
			if (GetParent()->GetText() != nullptr)
			{
				Text* text = GetParent()->GetText();
				static Vector4 lastFrameTextTint = Vector4(1, 1, 1, 1);
				static Vector2 lastFrameOffset = Vector2(0, 0);	
				int textFrameCounter = 0;

				for (std::vector<std::shared_ptr<S_Text>>::iterator frame = props->textProps.begin(); frame != props->textProps.end(); frame++)
				{
					float keyframeTime = (*frame)->time;		
					std::shared_ptr<S_Text> thisFrameProps = (*frame);
					std::vector<std::shared_ptr<S_Text>>::iterator lastFrame = frame;
					bool b_fontPathAnimated = thisFrameProps->b_fontPathAnimated;
					bool b_textAnimated = thisFrameProps->b_textAnimated;
					bool b_tintColorAnimated = thisFrameProps->b_tintColorAnimated;
					bool b_offsetAnimated = thisFrameProps->b_offsetAnimated;

					float lastFrameTime = 0;
					if (textFrameCounter > 0 && props->textProps.size() > 1)
					{
						lastFrame--;
						lastFrameTime = (*lastFrame)->time;
					}

					if (keyframeTime == 0 && !(*frame)->b_fired)
					{
						if (thisFrameProps->b_textAnimated)
						{
							text->SetText(thisFrameProps->text);							
						}
						if (b_offsetAnimated)
						{
							text->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
						}
						if (b_fontPathAnimated && thisFrameProps->fontPath != "")
						{
							text->SetFontPath(thisFrameProps->fontPath);
						}
						
						if (b_tintColorAnimated)
						{
							text->SetColor(thisFrameProps->tintColor);
						}
						thisFrameProps->b_fired = true;
					}
					else if ((ellapsedTime > lastFrameTime + m_animationStartTime) && (ellapsedTime < m_animationStartTime + keyframeTime))
					{
						std::shared_ptr<S_Text> lastFrameProps = (*lastFrame);
						float timeLeft = (m_animationStartTime + keyframeTime) - ellapsedTime;
						float percentDone = (float)(ellapsedTime - m_animationStartTime - lastFrameTime) / (keyframeTime - lastFrameTime);
						
						if (!thisFrameProps->b_fired && b_textAnimated)
						{
							text->SetText(thisFrameProps->text);
							text->LoadText();
							thisFrameProps->b_fired = true;
						}
						if (b_tintColorAnimated)
						{
							Vector4 correctedTintColor = thisFrameProps->tintColor;
							lastFrameTextTint = (*lastFrame)->tintColor;
							if (!thisFrameProps->b_instantTintChange)
							{
								correctedTintColor = Vector4(lastFrameTextTint.x + (thisFrameProps->tintColor.x - lastFrameTextTint.x) * percentDone,
									lastFrameTextTint.y + (thisFrameProps->tintColor.y - lastFrameTextTint.y) * percentDone,
									lastFrameTextTint.z + (thisFrameProps->tintColor.z - lastFrameTextTint.z) * percentDone,
									lastFrameTextTint.w + (thisFrameProps->tintColor.w - lastFrameTextTint.w) * percentDone);
							}
							text->SetColor(correctedTintColor);
						}
						if (b_fontPathAnimated && thisFrameProps->fontPath != "")
						{
							text->SetFontPath(thisFrameProps->fontPath);
						}					
						if (b_offsetAnimated)
						{
							lastFrameOffset = Vector2(lastFrameProps->xOffset, lastFrameProps->yOffset);
							text->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
						}									
					}
					textFrameCounter++;
				}
			}

			// Audio Animation Frames
			if (GetParent()->GetAudio() != nullptr)
			{
				Audio* audio = GetParent()->GetAudio();
				int audioFrameCounter = 0;

				for (std::vector<std::shared_ptr<S_Audio>>::iterator frame = props->audioProps.begin(); frame != props->audioProps.end(); frame++)
				{				
					float keyframeTime = (*frame)->time;				
					std::shared_ptr<S_Audio> thisFrameProps = (*frame);
					std::vector<std::shared_ptr<S_Audio>>::iterator nextFrame = frame;
					bool b_stopsAllOtherSounds = thisFrameProps->b_stopAllOtherSounds;

					float nextFrameTime = 0;
					if (audioFrameCounter > 0 && props->audioProps.size() > 1)
					{
						nextFrame++;
						if (nextFrame != props->audioProps.end())
						{
							nextFrameTime = (*nextFrame)->time;
						}
					}

					if (keyframeTime == 0 && !(*frame)->b_fired)
					{
						if (b_stopsAllOtherSounds)
						{							
							audio->StopAll();
						}
						
						audio->PlaySound(thisFrameProps->soundName);
						thisFrameProps->b_fired = true;
					}
					else if (!thisFrameProps->b_fired && (ellapsedTime > m_animationStartTime + keyframeTime))
					{
						if (b_stopsAllOtherSounds)
						{
							audio->StopAll();
						}
						audio->PlaySound(thisFrameProps->soundName);
						thisFrameProps->b_fired = true;
					}
					audioFrameCounter++;
				}
			}
		}
		else if (props->b_loop)
		{
			m_animationStartTime = ellapsedTime;
		}
		else
		{
			Stop();
		}
	}
}