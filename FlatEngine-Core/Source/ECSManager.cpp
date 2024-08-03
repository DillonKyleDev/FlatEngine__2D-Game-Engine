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
		m_Transforms = std::vector<std::pair<Transform, long>>();
		m_Sprites = std::vector<std::pair<Sprite, long>>();
		m_Cameras = std::vector<std::pair<Camera, long>>();
		m_Scripts = std::vector<std::pair<ScriptComponent, long>>();
		m_GameScripts = std::vector<std::pair<GameScript, long>>();
		m_Buttons = std::vector<std::pair<Button, long>>();
		m_Canvases = std::vector<std::pair<Canvas, long>>();
		m_Animations = std::vector<std::pair<Animation, long>>();
		m_Audios = std::vector<std::pair<Audio, long>>();
		m_Texts = std::vector<std::pair<Text, long>>();
		m_Colliders = std::vector<std::pair<Collider, long>>();
		m_CompositeColliders = std::vector<std::pair<CompositeCollider, long>>();
		m_BoxColliders = std::vector<std::pair<BoxCollider, long>>();
		m_CircleColliders = std::vector<std::pair<CircleCollider, long>>();
		m_RigidBodies = std::vector<std::pair<RigidBody, long>>();
		m_CharacterControllers = std::vector<std::pair<CharacterController, long>>();

		m_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();

		m_transformMap = std::map<long, long>();
		m_spriteMap = std::map<long, long>();
		m_cameraMap = std::map<long, long>();
		m_scriptMap = std::map<long, std::vector<long>>();
		m_gameScriptMap = std::map<long, long>();
		m_buttonMap = std::map<long, long>();
		m_canvasMap = std::map<long, long>();
		m_animationMap = std::map<long, long>();
		m_audioMap = std::map<long, long>();
		m_textMap = std::map<long, long>();
		m_colliderMap = std::map<long, long>();
		m_compositeColliderMap = std::map<long, long>();
		m_boxColliderMap = std::map<long, long>();
		m_circleColliderMap = std::map<long, long>();
		m_rigidBodyMap = std::map<long, long>();
		m_characterControllerMap = std::map<long, long>();
	}

	ECSManager::~ECSManager()
	{

	}

	Transform* ECSManager::AddTransform(Transform transform, long ownerID)
	{
		std::pair<Transform, long> newPair = { transform, ownerID };
		m_Transforms.push_back(newPair);
		m_transformMap.emplace(ownerID, (long)m_Transforms.size() - 1);
		return &m_Transforms[m_Transforms.size() - 1].first;
	}

	Sprite* ECSManager::AddSprite(Sprite sprite, long ownerID)
	{
		std::pair<Sprite, long> newPair = { sprite, ownerID };
		m_Sprites.push_back(newPair);
		m_spriteMap.emplace(ownerID, (long)m_Sprites.size() - 1);
		return &m_Sprites[m_Sprites.size() - 1].first;
	}

	Camera* ECSManager::AddCamera(Camera camera, long ownerID)
	{
		std::pair<Camera, long> newPair = { camera, ownerID };
		m_Cameras.push_back(newPair);
		m_cameraMap.emplace(ownerID, (long)m_Cameras.size() - 1);
		return &m_Cameras[m_Cameras.size() - 1].first;
	}

	ScriptComponent* ECSManager::AddScriptComponent(ScriptComponent script, long ownerID)
	{
		std::pair<ScriptComponent, long> newPair = { script, ownerID };
		m_Scripts.push_back(newPair);
		m_scriptMap.emplace(ownerID, (long)m_Scripts.size() - 1);
		return &m_Scripts[m_Scripts.size() - 1].first;
	}


	void ECSManager::CollectPhysicsBodies()
	{
		UpdateColliderPairs();
		UpdateActiveRigidBodies();
	}

	GameScript* ECSManager::AddScript(GameObject owner, ScriptComponent scriptComponent, GameScript scriptInstance)
	{
		// MUST FIX SOON
		scriptComponent.SetScriptInstance(&scriptInstance);
		scriptInstance.SetOwner(&owner);
		m_GameScripts.push_back(std::pair<GameScript, long>(scriptInstance, owner.GetID()));
		scriptInstance.Awake();
		scriptInstance.Start();
		return nullptr;
	}

	Canvas* ECSManager::AddCanvas(Canvas canvas, long ownerID)
	{
		std::pair<Canvas, long> newPair = { canvas, ownerID };
		m_Canvases.push_back(newPair);
		m_transformMap.emplace(ownerID, (long)m_Canvases.size() - 1);
		return &m_Canvases[m_Canvases.size() - 1].first;
	}

	Audio* ECSManager::AddAudio(Audio audio, long ownerID)
	{
		std::pair<Audio, long> newPair = { audio, ownerID };
		m_Audios.push_back(newPair);
		m_audioMap.emplace(ownerID, (long)m_Audios.size() - 1);
		return &m_Audios[m_Audios.size() - 1].first;
	}

	Text* ECSManager::AddText(Text text, long ownerID)
	{
		std::pair<Text, long> newPair = { text, ownerID };
		m_Texts.push_back(newPair);
		m_textMap.emplace(ownerID, (long)m_Texts.size() - 1);
		return &m_Texts[m_Texts.size() - 1].first;
	}

	Collider* ECSManager::AddCollider(Collider collider, long ownerID)
	{
		std::pair<Collider, long> newPair = { collider, ownerID };
		m_Colliders.push_back(newPair);
		m_colliderMap.emplace(ownerID, (long)m_Colliders.size() - 1);
		return &m_Colliders[m_Colliders.size() - 1].first;
	}

	CompositeCollider* ECSManager::AddCompositeCollider(CompositeCollider collider, long ownerID)
	{
		std::pair<CompositeCollider, long> newPair = { collider, ownerID };
		m_CompositeColliders.push_back(newPair);
		m_compositeColliderMap.emplace(ownerID, (long)m_CompositeColliders.size() - 1);
		return &m_CompositeColliders[m_CompositeColliders.size() - 1].first;
	}

	BoxCollider* ECSManager::AddBoxCollider(BoxCollider collider, long ownerID)
	{
		std::pair<BoxCollider, long> newPair = { collider, ownerID };
		m_BoxColliders.push_back(newPair);
		m_boxColliderMap.emplace(ownerID, (long)m_BoxColliders.size() - 1);
		return &m_BoxColliders[m_BoxColliders.size() - 1].first;
	}

	CircleCollider* ECSManager::AddCircleCollider(CircleCollider collider, long ownerID)
	{
		std::pair<CircleCollider, long> newPair = { collider, ownerID };
		m_CircleColliders.push_back(newPair);
		m_circleColliderMap.emplace(ownerID, (long)m_CircleColliders.size() - 1);
		return &m_CircleColliders[m_CircleColliders.size() - 1].first;
	}

	Animation* ECSManager::AddAnimation(Animation animation, long ownerID)
	{
		std::pair<Animation, long> newPair = { animation, ownerID };
		m_Animations.push_back(newPair);
		m_animationMap.emplace(ownerID, (long)m_Animations.size() - 1);
		return &m_Animations[m_Animations.size() - 1].first;
	}

	Button* ECSManager::AddButton(Button button, long ownerID)
	{
		std::pair<Button, long> newPair = { button, ownerID };
		m_Buttons.push_back(newPair);
		m_buttonMap.emplace(ownerID, (long)m_Buttons.size() - 1);
		return &m_Buttons[m_Buttons.size() - 1].first;
	}

	RigidBody* ECSManager::AddRigidBody(RigidBody rigidBody, long ownerID)
	{
		std::pair<RigidBody, long> newPair = { rigidBody, ownerID };
		m_RigidBodies.push_back(newPair);
		m_rigidBodyMap.emplace(ownerID, (long)m_RigidBodies.size() - 1);
		return &m_RigidBodies[m_RigidBodies.size() - 1].first;
	}

	CharacterController* ECSManager::AddCharacterController(CharacterController characterController, long ownerID)
	{
		std::pair<CharacterController, long> newPair = { characterController, ownerID };
		m_CharacterControllers.push_back(newPair);
		m_characterControllerMap.emplace(ownerID, (long)m_CharacterControllers.size() - 1);
		return &m_CharacterControllers[m_CharacterControllers.size() - 1].first;
	}

	Transform* ECSManager::GetTransform(long ownerID)
	{
		if (m_transformMap.count(ownerID) && m_Transforms.size() >= m_transformMap.at(ownerID))
			return &m_Transforms[m_transformMap.at(ownerID)].first;
		else return nullptr;
	}

	Sprite* ECSManager::GetSprite(long ownerID)
	{
		if (m_spriteMap.count(ownerID) && m_Sprites.size() >= m_spriteMap.at(ownerID))
			return &m_Sprites[m_spriteMap.at(ownerID)].first;
		else return nullptr;
	}

	Camera* ECSManager::GetCamera(long ownerID)
	{
		if (m_cameraMap.count(ownerID) && m_Cameras.size() >= m_cameraMap.at(ownerID))
			return &m_Cameras[m_cameraMap.at(ownerID)].first;
		else return nullptr;
	}

	ScriptComponent* ECSManager::GetScriptComponent(long ownerID)
	{
		if (m_scriptMap.count(ownerID) && m_Scripts.size() >= m_scriptMap.at(ownerID)[0])
			return &m_Scripts[m_scriptMap.at(ownerID)[0]].first;
		else return nullptr;
	}

	GameScript* ECSManager::GetScript(long ownerID, std::string name)
	{
		if (m_gameScriptMap.count(ownerID) && m_GameScripts.size() >= m_gameScriptMap.at(ownerID))
			return &m_GameScripts[m_gameScriptMap.at(ownerID)].first;
		else return nullptr;
	}

	Canvas* ECSManager::GetCanvas(long ownerID)
	{
		if (m_canvasMap.count(ownerID) && m_Canvases.size() >= m_canvasMap.at(ownerID))
			return &m_Canvases[m_canvasMap.at(ownerID)].first;
		else return nullptr;
	}

	Audio* ECSManager::GetAudio(long ownerID)
	{
		if (m_audioMap.count(ownerID) && m_Audios.size() >= m_audioMap.at(ownerID))
			return &m_Audios[m_audioMap.at(ownerID)].first;
		else return nullptr;
	}

	Text* ECSManager::GetText(long ownerID)
	{
		if (m_textMap.count(ownerID) && m_Texts.size() >= m_textMap.at(ownerID))
			return &m_Texts[m_textMap.at(ownerID)].first;
		else return nullptr;
	}

	Collider* ECSManager::GetCollider(long ownerID)
	{
		if (m_colliderMap.count(ownerID) && m_Colliders.size() >= m_colliderMap.at(ownerID))
			return &m_Colliders[m_colliderMap.at(ownerID)].first;
		else return nullptr;
	}

	CompositeCollider* ECSManager::GetCompositeCollider(long ownerID)
	{
		if (m_compositeColliderMap.count(ownerID) && m_CompositeColliders.size() >= m_compositeColliderMap.at(ownerID))
			return &m_CompositeColliders[m_compositeColliderMap.at(ownerID)].first;
		else return nullptr;
	}

	BoxCollider* ECSManager::GetBoxCollider(long ownerID)
	{
		if (m_boxColliderMap.count(ownerID) && m_BoxColliders.size() >= m_boxColliderMap.at(ownerID))
			return &m_BoxColliders[m_boxColliderMap.at(ownerID)].first;
		else return nullptr;
	}

	CircleCollider* ECSManager::GetCircleCollider(long ownerID)
	{
		if (m_circleColliderMap.count(ownerID) && m_CircleColliders.size() >= m_circleColliderMap.at(ownerID))
			return &m_CircleColliders[m_circleColliderMap.at(ownerID)].first;
		else return nullptr;
	}

	Animation* ECSManager::GetAnimation(long ownerID)
	{
		if (m_animationMap.count(ownerID) && m_Animations.size() >= m_animationMap.at(ownerID))
			return &m_Animations[m_animationMap.at(ownerID)].first;
		else return nullptr;
	}

	Button* ECSManager::GetButton(long ownerID)
	{
		if (m_buttonMap.count(ownerID) && m_Buttons.size() >= m_buttonMap.at(ownerID))
			return &m_Buttons[m_buttonMap.at(ownerID)].first;
		else return nullptr;
	}

	RigidBody* ECSManager::GetRigidBody(long ownerID)
	{
		if (m_rigidBodyMap.count(ownerID) && m_RigidBodies.size() >= m_rigidBodyMap.at(ownerID))
			return &m_RigidBodies[m_rigidBodyMap.at(ownerID)].first;
		else return nullptr;
	}

	CharacterController* ECSManager::GetCharacterController(long ownerID)
	{
		if (m_characterControllerMap.count(ownerID) && m_CharacterControllers.size() >= m_characterControllerMap.at(ownerID))
			return &m_CharacterControllers[m_characterControllerMap.at(ownerID)].first;
		else return nullptr;
	}

	void ECSManager::RemoveScript(long scriptID)
	{
		for (std::vector<std::pair<GameScript, long>>::iterator iter = m_GameScripts.begin(); iter != m_GameScripts.end();)
		{
			//if ((*iter).first.GetOwnerID() == scriptID)
			//{
			//	//RemoveProfilerProcess((*iter)->GetName() + "-on-" + (*iter)->GetOwner()->GetName());
			//	m_GameScripts.erase(iter);
			//	return;
			//}
			//iter++;
		}
	}

	void ECSManager::RemoveRigidBody(long rigidBodyID)
	{
		for (std::vector<std::pair<RigidBody, long>>::iterator iter = m_RigidBodies.begin(); iter != m_RigidBodies.end();)
		{
			if ((*iter).first.GetID() == rigidBodyID)
			{
				m_RigidBodies.erase(iter);
				return;
			}
			iter++;
		}
	}

	void ECSManager::RemoveCollider(long colliderID)
	{
		for (std::vector<std::pair<Collider, long>>::iterator iter = m_Colliders.begin(); iter != m_Colliders.end();)
		{
			if ((*iter).first.GetID() == colliderID)
			{
				m_Colliders.erase(iter);
				UpdateColliderPairs();
				return;
			}
			iter++;
		}
	}

	void ECSManager::InitializeScriptObjects(std::vector<GameObject> gameObjects)
	{
	}

	void ECSManager::UpdateColliderPairs()
	{
		// Get currently loaded scenes GameObjects
		//std::vector<GameObject> gameObjects = FlatEngine::GetSceneObjects();
		//m_Colliders.clear();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		//for (int i = 0; i < gameObjects.size(); i++)
		//{
		//	std::vector<Component*> components = gameObjects[i].GetComponents();

		//	for (int j = 0; j < components.size(); j++)
		//	{
		//		if (components[j]->GetTypeString() == "BoxCollider")
		//		{
		//			// Collect all BoxCollider components for collision detection in Update()
		//			m_Colliders.push_back(static_cast<BoxCollider>(components[j]));
		//		}
		//		if (components[j]->GetTypeString() == "CircleCollider")
		//		{
		//			// Collect all BoxCollider components for collision detection in Update()
		//			m_Colliders.push_back(static_cast<CircleCollider>(components[j]));
		//		}
		//		if (components[j]->GetTypeString() == "CompositeCollider")
		//		{
		//			// Collect all CompositeCollider components for collision detection in Update()
		//			m_Colliders.push_back(static_cast<CompositeCollider>(components[j]));
		//		}
		//	}
		//}

		// Remake colliderPairs
		m_ColliderPairs.clear();

		for (std::vector<std::pair<Collider, long>>::iterator iterOuter = m_Colliders.begin(); iterOuter != m_Colliders.end();)
		{
			for (std::vector<std::pair<Collider, long>>::iterator iterInner = m_Colliders.begin(); iterInner != m_Colliders.end();)
			{
				// If colliders don't belong to the same GameObject
				if (iterOuter->first.GetParentID() != (*iterInner).first.GetParentID() && (*iterOuter).first.GetTypeString() != "CompositeCollider" && (*iterInner).first.GetTypeString() != "CompositeCollider")
				{
					TagList coll1TagList = (*iterOuter).first.GetParent()->GetTagList();
					TagList coll2TagList = (*iterInner).first.GetParent()->GetTagList();

					std::vector<std::string> coll1Ignored = coll1TagList.GetIgnoredTags();
					std::vector<std::string> coll2Ignored = coll2TagList.GetIgnoredTags();

					bool _ignorePair = false;

					for (std::string ignoredTag : coll1Ignored)
					{
						if (coll2TagList.HasTag(ignoredTag))
						{
							_ignorePair = true;
							break;
						}
					}
					if (!_ignorePair)
					{
						for (std::string ignoredTag : coll2Ignored)
						{
							if (coll1TagList.HasTag(ignoredTag))
							{
								_ignorePair = true;
								break;
							}
						}
					}
					if (!_ignorePair)
					{
						std::pair<Collider*, Collider*> newPair = { &(*iterOuter).first, &(*iterInner).first };
						m_ColliderPairs.push_back(newPair);
					}
				}
				iterInner++;
			}
			iterOuter++;
		}
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

	std::vector<std::pair<Transform, long>> ECSManager::GetTransforms()
	{
		return m_Transforms;
	}
	std::vector<std::pair<Sprite, long>> ECSManager::GetSprites()
	{
		return m_Sprites;
	}
	std::vector<std::pair<Camera, long>> ECSManager::GetCameras()
	{
		return m_Cameras;
	}
	std::vector<std::pair<ScriptComponent, long>> ECSManager::GetScriptComponents()
	{
		return m_Scripts;
	}
	std::vector<std::pair<GameScript, long>> ECSManager::GetScripts()
	{
		return m_GameScripts;
	}
	std::vector<std::pair<Button, long>> ECSManager::GetButtons()
	{
		return m_Buttons;
	}
	std::vector<std::pair<Canvas, long>> ECSManager::GetCanvases()
	{
		return m_Canvases;
	}
	std::vector<std::pair<Animation, long>> ECSManager::GetAnimations()
	{
		return m_Animations;
	}
	std::vector<std::pair<Audio, long>> ECSManager::GetAudios()
	{
		return m_Audios;
	}
	std::vector<std::pair<Text, long>> ECSManager::GetTexts()
	{
		return m_Texts;
	}
	std::vector<std::pair<Collider, long>> ECSManager::GetColliders()
	{
		return m_Colliders;
	}
	std::vector<std::pair<CompositeCollider, long>> ECSManager::GetCompositeColliders()
	{
		return m_CompositeColliders;
	}
	std::vector<std::pair<BoxCollider, long>> ECSManager::GetBoxColliders()
	{
		return m_BoxColliders;
	}
	std::vector<std::pair<CircleCollider, long>> ECSManager::GetCircleColliders()
	{
		return m_CircleColliders;
	}
	std::vector<std::pair<RigidBody, long>> &ECSManager::GetRigidBodies()
	{
		return m_RigidBodies;
	}
	std::vector<std::pair<CharacterController, long>> ECSManager::GetCharacterControllers()
	{
		return m_CharacterControllers;
	}
}