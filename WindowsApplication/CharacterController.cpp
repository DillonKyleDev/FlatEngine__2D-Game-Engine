#include "CharacterController.h"
#include "FlatEngine.h"


namespace FlatEngine {
	CharacterController::CharacterController(long myID, long parentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(myID);
		SetParentID(parentID);
	}

	CharacterController::CharacterController(std::shared_ptr<CharacterController> toCopy, long newParentID)
	{
		SetType(ComponentTypes::CharacterController);
		SetID(FlatEngine::GetNextComponentID());
		SetParentID(newParentID);
	}

	CharacterController::~CharacterController()
	{
	}

	std::string CharacterController::GetData()
	{
		json jsonData = {
			{ "type", "BoxCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}