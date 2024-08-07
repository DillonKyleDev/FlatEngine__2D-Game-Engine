#include "ECSManager.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "ScriptComponent.h"
#include "GameScript.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "Collider.h"
#include "CompositeCollider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "CharacterController.h"

namespace FlatEngine
{
	ECSManager::ECSManager()
	{
		m_Transforms = std::map<long, Transform>();
		m_Sprites = std::map<long, Sprite>();
		m_Cameras = std::map<long, Camera>();
		m_Scripts = std::map<long, std::map<long, ScriptComponent>>();
		m_GameScripts = std::map<long, GameScript>();
		m_Buttons = std::map<long, Button>();
		m_Canvases = std::map<long, Canvas>();
		m_Animations = std::map<long, Animation>();
		m_Audios = std::map<long, Audio>();
		m_Texts = std::map<long, Text>();
		m_CompositeColliders = std::map<long, CompositeCollider>();
		m_BoxColliders = std::map<long, std::map<long, BoxCollider>>();
		m_CircleColliders = std::map<long, CircleCollider>();
		m_RigidBodies = std::map<long, RigidBody>();
		m_CharacterControllers = std::map<long, CharacterController>();

		m_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();
	}

	ECSManager::~ECSManager()
	{
	}

	Transform* ECSManager::AddTransform(Transform transform, long ownerID)
	{
		m_Transforms.emplace(ownerID, transform);
		return &m_Transforms.at(ownerID);
	}

	Sprite* ECSManager::AddSprite(Sprite sprite, long ownerID)
	{
		m_Sprites.emplace(ownerID, sprite);
		return &m_Sprites.at(ownerID);
	}

	Camera* ECSManager::AddCamera(Camera camera, long ownerID)
	{		
		m_Cameras.emplace(ownerID, camera);
		return &m_Cameras.at(ownerID);
	}

	ScriptComponent* ECSManager::AddScriptComponent(ScriptComponent script, long ownerID)
	{
		std::map<long, ScriptComponent> newMap;
		std::pair<long, ScriptComponent> newPair = { script.GetID(), script };

		if (m_Scripts.count(ownerID))
		{
			m_Scripts.at(ownerID).emplace(newPair);
		}
		else
		{
			newMap.emplace(newPair);
			m_Scripts.emplace(ownerID, newMap);
		}

		return &m_Scripts.at(ownerID).at(script.GetID());
	}


	void ECSManager::CollectPhysicsBodies()
	{
		UpdateColliderPairs();
		UpdateActiveRigidBodies();
	}

	GameScript* ECSManager::AddScript(GameObject owner, ScriptComponent scriptComponent, GameScript scriptInstance)
	{
		// MUST FIX SOON 
		// Instead of having it just be a map<long, GameScript>, make it like the m_Scripts with map<long, map<long, GameScript>>
		// WILL NEED TO ADD ID member variable to GameScripts.
		scriptComponent.SetScriptInstance(&scriptInstance);
		scriptInstance.SetOwner(&owner);
		m_GameScripts.emplace(owner.GetID(), scriptInstance);
		scriptInstance.Awake();
		scriptInstance.Start();
		return &m_GameScripts.at(owner.GetID());
	}

	Canvas* ECSManager::AddCanvas(Canvas canvas, long ownerID)
	{
		m_Canvases.emplace(ownerID, canvas);
		return &m_Canvases.at(ownerID);
	}

	Audio* ECSManager::AddAudio(Audio audio, long ownerID)
	{
		m_Audios.emplace(ownerID, audio);
		return &m_Audios.at(ownerID);
	}

	Text* ECSManager::AddText(Text text, long ownerID)
	{
		m_Texts.emplace(ownerID, text);
		return &m_Texts.at(ownerID);
	}

	CompositeCollider* ECSManager::AddCompositeCollider(CompositeCollider collider, long ownerID)
	{
		m_CompositeColliders.emplace(ownerID, collider);
		return &m_CompositeColliders.at(ownerID);
	}

	BoxCollider* ECSManager::AddBoxCollider(BoxCollider collider, long ownerID)
	{
		std::map<long, BoxCollider> newMap;
		std::pair<long, BoxCollider> newPair = { collider.GetID(), collider };

		if (m_BoxColliders.count(ownerID))
		{
			m_BoxColliders.at(ownerID).emplace(newPair);
		}
		else
		{
			newMap.emplace(newPair);
			m_BoxColliders.emplace(ownerID, newMap);
		}

		return &m_BoxColliders.at(ownerID).at(collider.GetID());
	}

	CircleCollider* ECSManager::AddCircleCollider(CircleCollider collider, long ownerID)
	{
		m_CircleColliders.emplace(ownerID, collider);
		return &m_CircleColliders.at(ownerID);
	}

	Animation* ECSManager::AddAnimation(Animation animation, long ownerID)
	{
		m_Animations.emplace(ownerID, animation);
		return &m_Animations.at(ownerID);
	}

	Button* ECSManager::AddButton(Button button, long ownerID)
	{
		m_Buttons.emplace(ownerID, button);
		return &m_Buttons.at(ownerID);
	}

	RigidBody* ECSManager::AddRigidBody(RigidBody rigidBody, long ownerID)
	{
		m_RigidBodies.emplace(ownerID, rigidBody);
		return &m_RigidBodies.at(ownerID);
	}

	CharacterController* ECSManager::AddCharacterController(CharacterController characterController, long ownerID)
	{
		m_CharacterControllers.emplace(ownerID, characterController);
		return &m_CharacterControllers.at(ownerID);
	}

	// Get Components
	Transform* ECSManager::GetTransformByOwner(long ownerID)
	{
		if (m_Transforms.count(ownerID))
			return &m_Transforms.at(ownerID);
		else return nullptr;
	}

	Sprite* ECSManager::GetSpriteByOwner(long ownerID)
	{
		if (m_Sprites.count(ownerID))
			return &m_Sprites.at(ownerID);
		else return nullptr;
	}

	Camera* ECSManager::GetCameraByOwner(long ownerID)
	{
		if (m_Cameras.count(ownerID))
			return &m_Cameras.at(ownerID);
		else return nullptr;
	}

	std::vector<ScriptComponent*> ECSManager::GetScriptsByOwner(long ownerID)
	{
		std::vector<ScriptComponent*> scripts = std::vector<ScriptComponent*>();
		if (m_Scripts.count(ownerID))
		{			 
			for (std::map<long, ScriptComponent>::iterator iter = m_Scripts.at(ownerID).begin(); iter != m_Scripts.at(ownerID).end();)
			{
				scripts.push_back(&(*iter).second);
			}
		}
		return scripts;
	}

	GameScript* ECSManager::GetGameScriptByOwner(long ownerID, std::string name)
	{
		// FIX THIS LATER AS WELL
		//if (m_gameScriptMap.count(ownerID) && m_GameScripts.size() >= m_gameScriptMap.at(ownerID))
		//	return &m_GameScripts[m_gameScriptMap.at(ownerID)].first;
		return nullptr;
	}

	Canvas* ECSManager::GetCanvasByOwner(long ownerID)
	{
		if (m_Canvases.count(ownerID))
			return &m_Canvases.at(ownerID);
		else return nullptr;
	}

	Audio* ECSManager::GetAudioByOwner(long ownerID)
	{
		if (m_Audios.count(ownerID))
			return &m_Audios.at(ownerID);
		else return nullptr;
	}

	Text* ECSManager::GetTextByOwner(long ownerID)
	{
		if (m_Texts.count(ownerID))
			return &m_Texts.at(ownerID);
		else return nullptr;
	}

	CompositeCollider* ECSManager::GetCompositeColliderByOwner(long ownerID)
	{
		if (m_CompositeColliders.count(ownerID))
			return &m_CompositeColliders.at(ownerID);
		else return nullptr;
	}

	std::vector<BoxCollider*> ECSManager::GetBoxCollidersByOwner(long ownerID)
	{
		std::vector<BoxCollider*> colliders = std::vector<BoxCollider*>();
		if (m_BoxColliders.count(ownerID))
		{
			for (std::map<long, BoxCollider>::iterator iter = m_BoxColliders.at(ownerID).begin(); iter != m_BoxColliders.at(ownerID).end();)
			{
				colliders.push_back(&(*iter).second);
			}
		}
		return colliders;
	}

	CircleCollider* ECSManager::GetCircleColliderByOwner(long ownerID)
	{
		if (m_CircleColliders.count(ownerID))
			return &m_CircleColliders.at(ownerID);
		else return nullptr;
	}

	Animation* ECSManager::GetAnimationByOwner(long ownerID)
	{
		if (m_Animations.count(ownerID))
			return &m_Animations.at(ownerID);
		else return nullptr;
	}

	Button* ECSManager::GetButtonByOwner(long ownerID)
	{
		if (m_Buttons.count(ownerID))
			return &m_Buttons.at(ownerID);
		else return nullptr;
	}

	RigidBody* ECSManager::GetRigidBodyByOwner(long ownerID)
	{
		if (m_RigidBodies.count(ownerID))
			return &m_RigidBodies.at(ownerID);
		else return nullptr;
	}

	CharacterController* ECSManager::GetCharacterControllerByOwner(long ownerID)
	{
		if (m_CharacterControllers.count(ownerID))
			return &m_CharacterControllers.at(ownerID);
		else return nullptr;
	}

	// Remove Components
	bool ECSManager::RemoveComponent(Component *component, long ownerID)
	{
		if (component->GetTypeString() == "Transform")
		{
			return RemoveTransform(ownerID);
		}
		else if (component->GetTypeString() == "Sprite")
		{
			return RemoveSprite(ownerID);
		}
		else if (component->GetTypeString() == "Camera")
		{
			return RemoveCamera(ownerID);
		}
		else if (component->GetTypeString() == "ScriptComponent")
		{
			return RemoveScript(ownerID);
		}
		else if (component->GetTypeString() == "Canvas")
		{
			return RemoveCanvas(ownerID);
		}
		else if (component->GetTypeString() == "Audio")
		{
			return RemoveAudio(ownerID);
		}
		else if (component->GetTypeString() == "Text")
		{
			return RemoveText(ownerID);
		}
		else if (component->GetTypeString() == "CompositeCollider")
		{
			return RemoveCompositeCollider(ownerID);
		}
		else if (component->GetTypeString() == "BoxCollider")
		{
			return RemoveBoxCollider(ownerID);
		}
		else if (component->GetTypeString() == "CircleCollider")
		{
			return RemoveCircleCollider(ownerID);
		}
		else if (component->GetTypeString() == "RigidBody")
		{
			return RemoveRigidBody(ownerID);
		}
		else if (component->GetTypeString() == "CharacterController")
		{
			return RemoveCharacterController(ownerID);
		}
		else
			return false;
	}

	bool ECSManager::RemoveTransform(long ownerID)
	{
		bool b_success = false;
		if (m_Transforms.count(ownerID))
		{
			m_Transforms.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveSprite(long ownerID)
	{
		bool b_success = false;
		if (m_Sprites.count(ownerID))
		{
			m_Sprites.erase(ownerID);			
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCamera(long ownerID)
	{
		bool b_success = false;
		if (m_Cameras.count(ownerID))
		{
			m_Cameras.erase(ownerID);			
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveScript(long scriptID)
	{
		// TODO
		return false;
	}

	bool ECSManager::RemoveGameScript(long scriptID)
	{
		// TODO
		return false;
	}

	bool ECSManager::RemoveCanvas(long ownerID)
	{
		bool b_success = false;
		if (m_Canvases.count(ownerID))
		{
			m_Canvases.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveAudio(long ownerID)
	{
		bool b_success = false;
		if (m_Audios.count(ownerID))
		{
			m_Audios.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveText(long ownerID)
	{
		bool b_success = false;
		if (m_Texts.count(ownerID))
		{
			m_Texts.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCompositeCollider(long ownerID)
	{
		bool b_success = false;
		if (m_CompositeColliders.count(ownerID))
		{
			m_CompositeColliders.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveBoxCollider(long ownerID)
	{
		bool b_success = false;
		if (m_BoxColliders.count(ownerID))
		{
			m_BoxColliders.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCircleCollider(long ownerID)
	{
		bool b_success = false;
		if (m_CircleColliders.count(ownerID))
		{
			m_CircleColliders.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveAnimation(long ownerID)
	{
		bool b_success = false;
		if (m_Animations.count(ownerID))
		{
			m_Animations.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveButton(long ownerID)
	{
		// TODO
		return false;
	}

	bool ECSManager::RemoveRigidBody(long ownerID)
	{
		bool b_success = false;
		if (m_RigidBodies.count(ownerID))
		{
			m_RigidBodies.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCharacterController(long ownerID)
	{
		bool b_success = false;
		if (m_CharacterControllers.count(ownerID))
		{
			m_CharacterControllers.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}


	void ECSManager::InitializeScriptObjects(std::vector<GameObject> gameObjects)
	{
		// TODO if needed
	}

	void ECSManager::UpdateColliderPairs()
	{
		// Remake colliderPairs
		m_ColliderPairs.clear();

		//for (std::vector<std::pair<Collider, long>>::iterator iterOuter = m_Colliders.begin(); iterOuter != m_Colliders.end();)
		//{
		//	for (std::vector<std::pair<Collider, long>>::iterator iterInner = m_Colliders.begin(); iterInner != m_Colliders.end();)
		//	{
		//		// If colliders don't belong to the same GameObject
		//		if (iterOuter->first.GetParentID() != (*iterInner).first.GetParentID() && (*iterOuter).first.GetTypeString() != "CompositeCollider" && (*iterInner).first.GetTypeString() != "CompositeCollider")
		//		{
		//			TagList coll1TagList = (*iterOuter).first.GetParent()->GetTagList();
		//			TagList coll2TagList = (*iterInner).first.GetParent()->GetTagList();

		//			std::vector<std::string> coll1Ignored = coll1TagList.GetIgnoredTags();
		//			std::vector<std::string> coll2Ignored = coll2TagList.GetIgnoredTags();

		//			bool _ignorePair = false;

		//			for (std::string ignoredTag : coll1Ignored)
		//			{
		//				if (coll2TagList.HasTag(ignoredTag))
		//				{
		//					_ignorePair = true;
		//					break;
		//				}
		//			}
		//			if (!_ignorePair)
		//			{
		//				for (std::string ignoredTag : coll2Ignored)
		//				{
		//					if (coll1TagList.HasTag(ignoredTag))
		//					{
		//						_ignorePair = true;
		//						break;
		//					}
		//				}
		//			}
		//			if (!_ignorePair)
		//			{
		//				std::pair<Collider*, Collider*> newPair = { &(*iterOuter).first, &(*iterInner).first };
		//				m_ColliderPairs.push_back(newPair);
		//			}
		//		}
		//		iterInner++;
		//	}
		//	iterOuter++;
		//}
	}

	std::vector<std::pair<Collider*, Collider*>> ECSManager::GetColliderPairs()
	{
		return m_ColliderPairs;
	}

	void ECSManager::UpdateActiveRigidBodies()
	{
		// Get currently loaded scenes GameObjects
		//gameObjects = FlatEngine::GetSceneObjects();
		//rigidBodies.clear();

		//// Find all script components on Scene GameObjects and add those GameObjects
		//// to their corresponding script class entity vector members
		//for (int i = 0; i < gameObjects.size(); i++)
		//{
		//	std::vector<Component*> components = gameObjects[i].GetComponents();

		//	for (int j = 0; j < components.size(); j++)
		//	{
		//		if (components[j]->GetTypeString() == "RigidBody" && components[j]->IsActive())
		//		{
		//			// Collect all BoxCollider components for collision detection in Update()
		//			rigidBodies.push_back(static_cast<RigidBody*>(components[j]));
		//		}
		//	}
		//}
	}

	std::map<long, Transform> &ECSManager::GetTransforms()
	{
		return m_Transforms;
	}
	std::map<long, Sprite> &ECSManager::GetSprites()
	{
		return m_Sprites;
	}
	std::map<long, Camera> &ECSManager::GetCameras()
	{
		return m_Cameras;
	}
	std::map<long, std::map<long, ScriptComponent>> &ECSManager::GetScriptComponents()
	{
		return m_Scripts;
	}
	std::map<long, GameScript> &ECSManager::GetScripts()
	{
		return m_GameScripts;
	}
	std::map<long, Button> &ECSManager::GetButtons()
	{
		return m_Buttons;
	}
	std::map<long, Canvas> &ECSManager::GetCanvases()
	{
		return m_Canvases;
	}
	std::map<long, Animation> &ECSManager::GetAnimations()
	{
		return m_Animations;
	}
	std::map<long, Audio> &ECSManager::GetAudios()
	{
		return m_Audios;
	}
	std::map<long, Text> &ECSManager::GetTexts()
	{
		return m_Texts;
	}
	std::vector<Collider*> ECSManager::GetColliders()
	{
		// Collect all colliders when needed so we don't have to keep an member variable updated
		std::vector<Collider*> colliders;
		for (std::pair<long, CompositeCollider> collider : m_CompositeColliders)
		{
			colliders.push_back(&collider.second);
		}
		for (std::pair<long, std::map<long, BoxCollider>> colliderMap : m_BoxColliders)
		{
			for (std::pair<long, BoxCollider> collider : colliderMap.second)
			{
				colliders.push_back(&collider.second);
			}
		}
		for (std::pair<long, CircleCollider> collider : m_CircleColliders)
		{
			colliders.push_back(&collider.second);
		}
		return colliders;
	}
	std::map<long, CompositeCollider> &ECSManager::GetCompositeColliders()
	{
		return m_CompositeColliders;
	}
	std::map<long, std::map<long, BoxCollider>> &ECSManager::GetBoxColliders()
	{
		return m_BoxColliders;
	}
	std::map<long, CircleCollider> &ECSManager::GetCircleColliders()
	{
		return m_CircleColliders;
	}
	std::map<long, RigidBody> &ECSManager::GetRigidBodies()
	{
		return m_RigidBodies;
	}
	std::map<long, CharacterController> &ECSManager::GetCharacterControllers()
	{
		return m_CharacterControllers;
	}
}