#include "Animation.h"
#include "FlatEngine.h"



namespace FlatEngine
{
	Animation::Animation(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Animation);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->ticksPerFrame = 10;
		this->frames = std::vector<std::shared_ptr<GameObject>>();
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
		//std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(this->GetParentID());

		//thisObject = std::make_shared<GameObject>();

		//FlatEngine::LogString("Old Name: " + this->frames[0]->GetName());
	}

	void Animation::Stop()
	{
	}

	void Animation::SetTicksPerFrame(float ticksPerFrame)
	{
		this->ticksPerFrame = ticksPerFrame;
	}

	float Animation::GetTicksPerFrame()
	{
		return this->ticksPerFrame;
	}

	std::vector<std::shared_ptr<GameObject>> Animation::GetFrames()
	{
		return this->frames;
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
}