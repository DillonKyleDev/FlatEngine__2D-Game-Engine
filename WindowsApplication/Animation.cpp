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
	Animation::~Animation()
	{
	}

	void Animation::AddFrame()
	{

	}

	void Animation::Play()
	{
		this->_playing = true;
		// Start animation timer
		this->animationStartTime = FlatEngine::GetEllapsedGameTime();
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
			{ "ticksPerFrame", this->ticksPerFrame }
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

	void Animation::PlayAnimation()
	{
		animationProperties = FlatEngine::FlatGui::LoadAnimationFile(animationPath);
		std::shared_ptr<S_AnimationProperties> props = animationProperties;

		// While the animation is not over
		if (props->animationLength > FlatEngine::GetEllapsedGameTime() - animationStartTime)
		{
			// Transform Animation Frames
			for (const S_Transform &transformFrame : props->transformProperties)
			{ 
				// Save original position
				std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
				static Vector2 startingPoint = transform->GetPosition();
				static Vector2 startingScale = transform->GetScale();

				if (animationStartTime + transformFrame.time > FlatEngine::GetEllapsedGameTime())
				{
					transform->SetPosition(Vector2(startingPoint.x + transformFrame.xMove, startingPoint.y + transformFrame.yMove));
					transform->SetScale(Vector2(startingScale.x + transformFrame.xScale, startingScale.y + transformFrame.yScale));
				}
			}
			// Sprite Animation Frames
			for (const S_Sprite& spriteFrame : props->spriteProperties)
			{
				std::shared_ptr<FlatEngine::Sprite> sprite = GetParent()->GetSpriteComponent();

				if (animationStartTime + spriteFrame.time > FlatEngine::GetEllapsedGameTime())
				{
					if (spriteFrame.path != "")
						sprite->SetTexture(spriteFrame.path);
					
					Vector2 spriteOffset = sprite->GetOffset();
					sprite->SetOffset(Vector2(spriteOffset.x + spriteFrame.xOffset, spriteOffset.y + spriteFrame.yOffset));
				}
			}
		}
		else
			this->Stop();
	}
}