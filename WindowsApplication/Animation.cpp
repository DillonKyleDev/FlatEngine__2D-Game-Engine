#include "Animation.h"
#include "FlatEngine.h"



namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Animation);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->_playing = false;
		this->ticksPerFrame = 10;
		this->animationStartTime = -1;
	}
	Animation::~Animation()
	{
	}

	void Animation::AddFrame()
	{
		//std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(this->GetParentID());
		//std::shared_ptr<GameObject> copiedState = std::make_shared<GameObject>(*thisObject);
		//this->frames.push_back(copiedState);

		//FlatEngine::LogString("From Add Frame. Added: " + frames[0]->GetName());
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
			{ "ticksPerFrame", this->ticksPerFrame },
			/*{ "frames", this->frames },*/
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
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
}