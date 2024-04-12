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

	Animation::Animation(std::shared_ptr<Animation> toCopy)
	{
		this->SetType(ComponentTypes::Animation);
		this->SetID(GetNextComponentID());
		this->SetParentID(toCopy->GetParentID());
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
		// NEED TO SORT ANIATION FRAMES BY TIME AND PLAY THEM IN THEIR OWN WINDOWS
		animationProperties = FlatEngine::FlatGui::LoadAnimationFile(animationPath);
		std::shared_ptr<S_AnimationProperties> props = animationProperties;

		// While the animation is not over
		if (props->animationLength > ellapsedTime - animationStartTime)
		{
			// Transform Animation Frames
			for (const std::shared_ptr<S_Transform> &transformFrame : props->transformProperties)
			{ 
				
				int timeLeft = transformFrame->time - ellapsedTime - animationStartTime;
				LogFloat(timeLeft, "Time LEft: ");
				float percentDone = ellapsedTime / transformFrame->time;

				// Save original position
				std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
				static Vector2 startingPoint = transform->GetPosition();
				static Vector2 startingScale = transform->GetScale();
				Vector2 currentPos = transform->GetPosition();
				Vector2 currentScale = transform->GetScale();

				if (animationStartTime + transformFrame->time > ellapsedTime)
				{
					switch (transformFrame->transformInterpType)
					{
						case Lerp:
						{
							LogFloat(percentDone, "Percent done: ");
							float correctedX = (startingPoint.x + transformFrame->xMove * percentDone);
							float correctedY = (startingPoint.y + transformFrame->yMove * percentDone);
							//transform->SetPosition(FlatEngine::Lerp(currentPos, Vector2(currentPos.x + correctedX, currentPos.y + correctedX), transformFrame->transformSpeed));
							transform->SetPosition(Vector2(correctedX, correctedY));
							break;
						}
					}
					switch (transformFrame->scaleInterpType)
					{
						case Lerp:
						{
							transform->SetScale(FlatEngine::Lerp(currentScale, Vector2(currentScale.x + transformFrame->xScale, currentScale.y + transformFrame->yScale), transformFrame->scaleSpeed));
							break;
						}
					}
				}
			}
			// Sprite Animation Frames
			for (const std::shared_ptr<S_Sprite>& spriteFrame : props->spriteProperties)
			{
				std::shared_ptr<FlatEngine::Sprite> sprite = GetParent()->GetSpriteComponent();

				if (animationStartTime + spriteFrame->time > ellapsedTime)
				{
					if (spriteFrame->path != "")
						sprite->SetTexture(spriteFrame->path);
					
					Vector2 spriteOffset = sprite->GetOffset();
					sprite->SetOffset(Vector2(spriteOffset.x + spriteFrame->xOffset, spriteOffset.y + spriteFrame->yOffset));
				}
			}
		}
		else
			this->Stop();
	}
}