#pragma once
#include <vector>
#include <map>

namespace FlatEngine
{
	class Transform;
	class Sprite;
	class ScriptComponent;
	class GameScript;
	class Canvas;
	class Audio;
	class Text;
	class Camera;
	class Collider;
	class BoxCollider;
	class CircleCollider;
	class Animation;
	class Button;
	class Sound;
	class RigidBody;
	class CompositeCollider;
	class CharacterController;

	class ECSManager
	{
	public:
		ECSManager();
		~ECSManager();

		std::vector<Transform> m_Transforms;
		std::vector<Sprite> m_Sprites;
		std::vector<Camera> m_Cameras;
		std::vector<ScriptComponent> m_Scripts;
		std::vector<GameScript> m_GameScripts;
		std::vector<Button> m_Buttons;
		std::vector<Canvas> m_Canvases;
		std::vector<Animation> m_Animations;
		std::vector<Audio> m_Audios;
		std::vector<Text> m_Texts;
		std::vector<Collider> m_Colliders;
		std::vector<CompositeCollider> m_CompositeColliders;
		std::vector<BoxCollider> m_BoxColliders;
		std::vector<CircleCollider> m_CircleColliders;
		std::vector<RigidBody> m_RigidBodies;
		std::vector<CharacterController> m_CharacterControllers;

		std::map<long, long> m_transformMap;
		std::map<long, long> m_spriteMap;
		std::map<long, long> m_cameraMap;
		std::map<long, std::vector<long>> m_scriptMap;
		std::map<long, long> m_gameScriptMap; // To be determined later
		std::map<long, long> m_buttonMap;
		std::map<long, long> m_canvasMap;
		std::map<long, long> m_animationMap;
		std::map<long, long> m_audioMap;
		std::map<long, long> m_textMap;
		std::map<long, long> m_colliderMap;
		std::map<long, long> m_compositeColliderMap;
		std::map<long, long> m_boxColliderMap;
		std::map<long, long> m_circleColliderMap;
		std::map<long, long> m_rigidBodyMap;
		std::map<long, long> m_characterControllerMap;
	};
}