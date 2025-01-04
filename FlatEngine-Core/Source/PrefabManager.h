#pragma once
#include "Vector2.h"
#include "TagList.h"
#include "Animation.h"
#include "Audio.h"    // SoundData
#include "TileMap.h"  // Tile
#include "Scene.h"

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;

namespace FlatEngine 
{
	class GameObject;

	struct GameObjectPrefabData {
		long parentID = -1;
		long ID = -1;
		bool b_isActive = true;
		std::string name = "";
		TagList tagList;
		std::vector<long> componentIDs;
		std::vector<long> childrenIDs;
	};
	struct ComponentPrefabData {
		std::string type;
		long ownerId = -1;
		long id = -1;
		bool b_isActive = true;
		bool b_isCollapsed = false;
	};

	struct TransformPrefabData : public ComponentPrefabData {
		Vector2 origin;
		Vector2 position;
		Vector2 scale;
		float rotation;
	};
	struct SpritePrefabData : public ComponentPrefabData {		
		int renderOrder;
		float textureWidth;
		float textureHeight;
		Vector2 scale;
		std::string pivotPoint;
		Vector2 pivotOffset;
		Vector2 offset;
		std::string path;
		Vector4 tintColor;
	};
	struct CameraPrefabData : public ComponentPrefabData {		
		float width;
		float height;
		float zoom;
		ImVec4 frustrumColor;
		bool b_isPrimaryCamera;
		bool b_shouldFollow;
		long toFollowID;
		float followSmoothing;
	};
	struct ScriptPrefabData : public ComponentPrefabData {		
		std::string attachedScript;
		std::map<std::string, Animation::S_EventFunctionParam> scriptParams;
	};
	struct ButtonPrefabData : public ComponentPrefabData {
		Vector2 activeDimensions;
		Vector2 activeOffset;
		int activeLayer;
	};
	struct CanvasPrefabData : public ComponentPrefabData {		
		int layerNumber;
		bool b_blocksLayers;
		float width;
		float height;
	};
	struct AnimationPrefabData : public ComponentPrefabData {
		std::vector<Animation::AnimationData> animations;
	};
	struct AudioPrefabData : public ComponentPrefabData {
		std::vector<SoundData> sounds;
	};
	struct TextPrefabData : public ComponentPrefabData {
		std::string fontPath;
		int fontSize;
		std::string text;
		Vector4 color;
		Vector2 offset;
		int renderOrder;
	};
	struct BoxColliderPrefabData : public ComponentPrefabData {		
		Vector2 activeDimensions;
		Vector2 activeOffset;
		int activeLayer;
		float rotation;
		bool b_tileMapCollider;
		bool b_isContinuous;
		bool b_isStatic;
		bool b_isSolid;
		bool b_showActiveRadius;
		bool b_isComposite;
	};
	struct CircleColliderPrefabData : public ComponentPrefabData {
		float activeRadius;
		Vector2 activeOffset;
		bool b_isContinuous;
		bool b_isStatic;
		bool b_isSolid;
		bool b_isComposite;
		int activeLayer;
	};
	struct RigidBodyPrefabData : public ComponentPrefabData {		
		float mass;
		float friction;		
		float I;
		float angularDrag;
		bool b_allowTorques;
		float forceCorrection;
		bool b_isStatic;
		float windResistance;
		float equilibriumForce;
		float gravity;
		float fallingGravity;
		float terminalVelocity;
	};
	struct CharacterControllerPrefabData : public ComponentPrefabData {
		float maxAcceleration;
		float maxSpeed;
		float airControl;
	};
	struct TileMapPrefabData : public ComponentPrefabData {
		int width;
		int height;
		int tileWidth;
		int tileHeight;
		int renderOrder;
		std::map<int, std::map<int, Tile>> tiles;
		std::vector<std::string> tileSetNames;
		std::map<std::string, std::vector<std::pair<Vector2, Vector2>>> collisionAreas;
	};

	struct Prefab {
		std::string name;
		Vector2 spawnLocation;
		GameObjectPrefabData rootObject;
		std::map<long, GameObjectPrefabData> objects;
		std::map<long, std::shared_ptr<ComponentPrefabData>> components;
	};

	class PrefabManager
	{
	public:
		PrefabManager();
		~PrefabManager();
		void CreatePrefabFromJson(json objectJson, Prefab &prefab);
		void CreatePrefab(std::string path, GameObject gameObject);
		void UpdatePrefab(std::string prefabName);
		void AddPrefab(std::string path);
		void InitializePrefabs();
		GameObject* InstantiateSelfAndChildren(long parentID, long myID, Prefab prefab, Scene* scene, Vector2 spawnLocation = Vector2(0, 0));
		GameObject* Instantiate(std::string prefabName, Vector2 spawnLocation, Scene* scene, long parentID = -1, long ID = -1);
		std::map<std::string, Prefab> GetPrefabs();

	private:
		std::map<std::string, Prefab> m_prefabs;
	};
}