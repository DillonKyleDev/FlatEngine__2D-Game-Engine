#include "ECSManager.h"
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
		m_Transforms = std::vector<Transform>();
		m_Sprites = std::vector<Sprite>();
		m_Cameras = std::vector<Camera>();
		m_Scripts = std::vector<ScriptComponent>();
		m_GameScripts = std::vector<GameScript>();
		m_Buttons = std::vector<Button>();
		m_Canvases = std::vector<Canvas>();
		m_Animations = std::vector<Animation>();
		m_Audios = std::vector<Audio>();
		m_Texts = std::vector<Text>();
		m_Colliders = std::vector<Collider>();
		m_CompositeColliders = std::vector<CompositeCollider>();
		m_BoxColliders = std::vector<BoxCollider>();
		m_CircleColliders = std::vector<CircleCollider>();
		m_RigidBodies = std::vector<RigidBody>();
		m_CharacterControllers = std::vector<CharacterController>();

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
}