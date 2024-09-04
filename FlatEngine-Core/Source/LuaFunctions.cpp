#include "FlatEngine.h"
#include "Vector2.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Canvas.h"
#include "Button.h"
#include "Audio.h"
#include "Animation.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Script.h"
#include "MappingContext.h"

#include <fstream>


// Lua helpers + Functions that Lua can call
namespace FlatEngine
{
	sol::state F_Lua;	
	std::vector<std::string> F_luaScriptPaths = std::vector<std::string>();
	std::vector<std::string> F_luaScriptNames = std::vector<std::string>();
	std::map<std::string, std::string> F_LuaScriptsMap = std::map<std::string, std::string>();

	void InitLua()
	{
		F_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
		RegisterLuaFunctions();
		RegisterLuaTypes();
	}

	// Inject functions that can be called from within Lua directly into the Lua state
	void RegisterLuaFunctions()
	{
		F_Lua["LogString"] = [](std::string line)
		{
			std::string prefix = "[LUA] " + F_Lua["calling_script_name"].get_or<std::string>("Script") + " :";
			LogString(line, prefix);
		};
		F_Lua["LogInt"] = [](int value, std::string line)
		{
			std::string prefix = "[LUA] " + F_Lua["calling_script_name"].get_or<std::string>("Script") + " :";
			LogInt(value, line, prefix);
		};
		F_Lua["LogFloat"] = [](float value, std::string line)
		{
			std::string prefix = "[LUA] " + F_Lua["calling_script_name"].get_or<std::string>("Script") + " :";
			LogFloat(value, line, prefix);
		};
		F_Lua["LogVector2"] = [](float xValue, float yValue, std::string line)
		{
			std::string prefix = "[LUA] " + F_Lua["calling_script_name"].get_or<std::string>("Script") + " :";
			LogVector2(Vector2(xValue, yValue), line, prefix);
		};
		F_Lua["GetMappingContext"] = [](std::string contextName)
		{
			return GetMappingContext(contextName);
		};
		F_Lua["Instantiate"] = [](std::string prefabName, Vector2 position)
		{
			return Instantiate(prefabName, position);
		};
	}

	// Map C++ types to Lua "Types"
	void RegisterLuaTypes()
	{
		F_Lua.new_usertype<Vector2>("Vector2",
			sol::constructors<Vector2(), Vector2(float x,float y)>(),
			"_x", &Vector2::SetX,
			"x", &Vector2::GetX,
			"_y", &Vector2::SetY,
			"y", &Vector2::GetY,
			"_xy", &Vector2::_xy
		);
		F_Lua.new_usertype<Vector4>("Vector4",
			sol::constructors<Vector4(), Vector4(float x, float y, float z, float w)>(),
			"_x", &Vector4::SetX,
			"x", &Vector4::GetX,
			"_y", &Vector4::SetY,
			"y", &Vector4::GetY,
			"_z", &Vector4::SetZ,
			"z", &Vector4::GetZ,
			"_w", &Vector4::SetW,
			"w", &Vector4::GetW,
			"_xyzw", &Vector4::_xyzw
		);

		F_Lua.new_usertype<GameObject>("GameObject",
			"GetID", &GameObject::GetID,
			"GetName", &GameObject::GetName,
			"SetName", &GameObject::SetName,
			"IsActive", &GameObject::IsActive,
			"SetActive", &GameObject::SetActive,
			"HasTag", &GameObject::HasTag,
			"GetTransform", &GameObject::GetTransform,
			"AddSprite", &GameObject::AddTransform,
			"GetSprite", &GameObject::GetSprite,
			"GetCamera", &GameObject::GetCamera,
			"GetAnimation", &GameObject::GetAnimation,
			"GetAudio", &GameObject::GetAudio,
			"GetButton", &GameObject::GetButton,
			"GetCanvas", &GameObject::GetCanvas,
			"GetText", &GameObject::GetText,
			"GetCharacterController", &GameObject::GetCharacterController,
			"GetRigidBody", &GameObject::GetRigidBody,
			"GetBoxCollider", &GameObject::GetBoxColliders,
			"GetFirstChild", &GameObject::GetFirstChild,
			"HasChildren", &GameObject::HasChildren,
			"GetChildren", &GameObject::GetChildren
		);

		F_Lua.new_usertype<Transform>("Transform",
			"GetParent", &Transform::GetParent,
			"GetID", &Transform::GetID,
			"SetPosition", &Transform::SetPosition,
			"GetPosition", &Transform::GetPosition,
			"GetTruePosition", &Transform::GetTruePosition,
			"SetRotation", &Transform::SetRotation,
			"GetRotation", &Transform::GetRotation,
			"SetScale", &Transform::SetScale,
			"GetScale", &Transform::GetScale
		);

		F_Lua.new_usertype<Sprite>("Sprite",
			"SetActive", &Sprite::SetActive,
			"IsActive", &Sprite::IsActive,
			"GetParent", &Sprite::GetParent,
			"GetID", &Sprite::GetID,
			"SetTexture", &Sprite::SetTexture,
			"GetPath", &Sprite::GetPath,
			"SetScale", &Sprite::SetScale,
			"GetScale", &Sprite::GetScale,
			"GetTextureWidth", &Sprite::GetTextureWidth,
			"GetTextureHeight", &Sprite::GetTextureHeight,			
			"SetTintColor", &Sprite::SetTintColor,
			"GetTintColor", &Sprite::GetTintColor
		);

		F_Lua.new_usertype<Audio>("Audio",
			"GetParent", &Audio::GetParent,
			"SetActive", &Audio::SetActive,
			"IsActive", &Audio::IsActive,
			"GetID", &Audio::GetID,
			"IsMusicPlaying", &Audio::IsMusicPlaying,
			"PlaySound", &Audio::PlaySound,
			"PauseSound", &Audio::PauseSound,
			"StopSound", &Audio::PauseSound,
			"StopAll", &Audio::StopAll
		);

		F_Lua.new_usertype<Animation>("Animation",
			"GetParent", &Animation::GetParent,
			"SetActive", &Animation::SetActive,
			"IsActive", &Animation::IsActive,
			"GetID", &Animation::GetID,
			"Play", &Animation::Play,
			"Stop", &Animation::Stop,
			"SetAnimationPath", &Animation::SetAnimationPath			
		);

		F_Lua.new_usertype<RigidBody>("RigidBody",
			"SetActive", &RigidBody::SetActive,
			"IsActive", &RigidBody::IsActive,
			"GetParent", &RigidBody::GetParent,
			"GetID", &RigidBody::GetID,
			"SetMass", &RigidBody::SetMass,
			"GetMass", &RigidBody::GetMass,
			"SetGravity", &RigidBody::SetGravity,
			"GetGravity", &RigidBody::GetGravity,
			"SetFallingGravity", &RigidBody::SetFallingGravity,
			"GetFallingGravity", &RigidBody::GetFallingGravity,
			"SetFriction", &RigidBody::SetFriction,
			"GetFriction", &RigidBody::GetFriction,
			"SetAngularDrag", &RigidBody::SetAngularDrag,
			"GetAngularDrag", &RigidBody::GetAngularDrag,
			"SetAngularVelocity", &RigidBody::SetAngularVelocity,
			"GetAngularVelocity", &RigidBody::GetAngularVelocity,
			"SetTorquesAllowed", &RigidBody::SetTorquesAllowed,
			"TorquesAllowed", &RigidBody::TorquesAllowed,
			"AddForce", &RigidBody::AddForce,
			"AddTorque", &RigidBody::AddTorque,
			"SetPendingForces", &RigidBody::SetPendingForces,
			"GetPendingForces", &RigidBody::GetPendingForces,
			"SetTerminalVelocity", &RigidBody::SetTerminalVelocity,
			"GetTerminalVelocity", &RigidBody::GetTerminalVelocity
		);

		F_Lua.new_usertype<Collider>("Collider",
			"GetParent", &Collider::GetParent,
			"SetActive", &Collider::SetActive,
			"IsActive", &Collider::IsActive,
			"GetID", &Collider::GetID
		);

		F_Lua.new_usertype<CharacterController>("CharacterController",
			"MoveToward", &CharacterController::MoveToward,
			"GetParent", &CharacterController::GetParent,
			"SetActive", &CharacterController::SetActive,
			"IsActive", &CharacterController::IsActive,
			"GetID", &CharacterController::GetID
		);

		F_Lua.new_usertype<MappingContext>("MappingContext",
			"Fired", &MappingContext::Fired,
			"ActionPressed", &MappingContext::ActionPressed,
			"GetName", &MappingContext::GetName
		);
	}


	void RunLuaFuncOnAllScripts(std::string functionName)
	{
		for (std::pair<long, std::map<long, Script>> object : GetLoadedScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					std::string filepath = "";
					if (FL::F_LuaScriptsMap.count(script.second.GetAttachedScript()))
						filepath = F_LuaScriptsMap.at(script.second.GetAttachedScript());

					if (DoesFileExist(filepath))
					{
						GameObject* caller = GetObjectById(object.first);
						if (InitLuaScript(filepath, caller))
						{
							sol::protected_function func = F_Lua[functionName];
							auto calledFunction = func();

							if (calledFunction.valid())
							{				
								// Great!
							}
							else
							{
								sol::error err = calledFunction;
								LogString(err.what());
							}
						}
					}
				}
			}
		}
	}

	void RunAwakeAndStart()
	{
		RunLuaFuncOnAllScripts("Awake");
		RunLuaFuncOnAllScripts("Start");
	}

	void RetrieveLuaScriptPaths()
	{
		F_luaScriptPaths.clear();
		F_luaScriptNames.clear();
		F_LuaScriptsMap.clear();

		F_luaScriptNames.push_back(""); // Empty string for when Scripts don't have any selected script attached in RenderScriptComponent()
	
		std::vector<std::string> scriptPaths = FindAllFilesWithExtension(GetDir("projectDir"), ".scp.lua");
		for (std::string path : scriptPaths)
		{			
			F_luaScriptPaths.push_back(path);
			F_luaScriptNames.push_back(GetFilenameFromPath(path));
			F_LuaScriptsMap.emplace(GetFilenameFromPath(path), path);
		}
	}

	void CreateNewLuaScript(std::string filename, std::string path)
	{
		for (std::string scriptPath : F_luaScriptPaths)
		{
			if (filename == GetFilenameFromPath(scriptPath))
			{
				LogString("ERROR: Script name already taken.  Please enter a different name for your new lua script.");
				return;
			}
		}

		std::ofstream outfile;
		std::string filenameWExtention;

		if (path == "")
			filenameWExtention = GetDir("scripts") + "/" + filename + ".scp.lua";
		else
			filenameWExtention = path + "/" + filename + ".scp.lua";

		outfile.open(filenameWExtention, std::ios_base::app);
		outfile << 
			"-- " + filename + "\n\n\n" +
			"-- using Lua in FlatEngine: \n\n" +
			"-- ### please visit RegisterLuaTypes() in \"FlatEngine-Core/Source/LuaFunctions.cpp\" to see the classes, class methods, and variables exposed to Lua scripts ### \n" +
			"-- ### please visit RegisterLuaFunctions() in \"FlatEngine-Core/Source/LuaFunctions.cpp\" to see the FlatEngine functions exposed to Lua scripts              ### \n\n" +
			"-- use \"this_object\" to reference the object that is attached to this script \n" +
			"-- use \":\" to access member variables and functions of objects: object:member_variable ..or.. object::member_function() \n" +
			"-- to concatinate two or more strings, use two periods: \"..\"  LogString(\"Just add two periods between arguments like\"..string_variable_name) \n" +
			"-- to create new objects of type Type with construction parameters, use: Type:new(parameters,...)  \n\n\n" +

			"-- called on each script before Start() runs at the start of the gameloop (or upon instantiation) \n" +
			"function Awake() \n\n" +
			"end \n\n" +

			"-- called at the start of the gameloop after Awake() (or upon instantiation) \n" +
			"function Start() \n" +
			"     LogString(\"" + filename + " : Start() called on \"..this_object:GetName()) \n\n" +
			"end \n" +
			
			"--called once per gameloop frame \n" +
			"function Update() \n\n" +
			"end \n\n";

		RetrieveLuaScriptPaths();
	}

	bool CheckLuaScriptFile(std::string filePath)
	{
		try
		{
			F_Lua.safe_script_file(filePath);
			return true;
		}
		catch (const sol::error& err)
		{
			LogString("ERROR : Lua script failed to load");
			LogString(err.what());
			return false;
		}
	}

	// Checks that the script filepath is good and sends the Lua state contextual data
	bool InitLuaScript(std::string filePath, GameObject* caller)
	{
		if (CheckLuaScriptFile(filePath))
		{
			// Store this object inside the Lua state to be accessed by the next Lua function calls
			F_Lua["this_object"] = caller;
			// Store the name of the script being called in the Lua state (for hands-off named logging from Lua)
			F_Lua["calling_script_name"] = GetFilenameFromPath(filePath);
			return true;
		}
		else
			return false;
	}

	template <class T>
	void CallVoidLuaFunction(std::string functionName, T param)
	{
		sol::protected_function protectedFunc = F_Lua[functionName];
		if (protectedFunc)
		{
			auto result = protectedFunc(param);
			if (!result.valid())
			{
				sol::error err = result;
				LogString("ERROR : Something went wrong in Lua function: " + functionName + "()");
				LogString(err.what());
			}
		}
	}

	void CallLuaOnCollisionEnter(GameObject* caller, Collider* collidedWith)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string filePath = F_LuaScriptsMap.at(script->GetAttachedScript());
						if (InitLuaScript(filePath, caller))
							CallVoidLuaFunction<Collider*>("OnCollisionEnter", collidedWith);
					}
				}
			}
		}
	}

	void CallLuaOnCollisionLeave(GameObject* caller, Collider* collidedWith)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string filePath = F_LuaScriptsMap.at(script->GetAttachedScript());
						if (InitLuaScript(filePath, caller))
							CallVoidLuaFunction<Collider*>("OnCollisionLeave", collidedWith);
					}
				}
			}
		}
	}

	void CallLuaOnActiveCollision(GameObject* caller, Collider* collidedWith)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string filePath = F_LuaScriptsMap.at(script->GetAttachedScript());
						if (InitLuaScript(filePath, caller))
							CallVoidLuaFunction<Collider*>("OnActiveCollision", collidedWith);
					}
				}
			}
		}
	}
}

// Lua / Sol cheat sheet
// 
//// We can also inject variables directly into the Lua state as well
//F_Lua["newVariable"] = 56;
//
//// We can also inject entire scripts into the Lua state that can use global functions and variables
//// This creates a new lua function called newFunction(a)
//F_Lua.script(
//	"function newFunction(a)"
//	" return newVariable + a "
//	"end");
//
//// This then calls that function with the value of 2 passed in
//int result = F_Lua["newFunction"](2);
//LogInt(result, "Returned Value from new function: ");

// Getting values from Lua globals
//LogInt(F_Lua["b"].get_or(0));
//LogString(F_Lua["a"].get_or<std::string>("0"));
//// Existence checking
//sol::optional<int> anOptionalInt = F_Lua["b"];
//if (anOptionalInt)
//LogInt(anOptionalInt.value(), "an Optional int: ");
//// Workaround for above
//auto intVar = F_Lua["b"].get<std::optional<int>>();
//if (intVar)
//LogInt(intVar.value(), "Optional int 2: ");

//std::optional<sol::table> playerTable = F_Lua["player"];
//if (playerTable)
//{
//	for (const auto& entry : playerTable.value())
//	{
//		sol::object key = entry.first;
//		sol::object value = entry.second;

//		std::string sKey = key.as<std::string>(); // cast key as a string

//		if (sKey == "Name")
//			player.name = value.as<std::string>();
//		if (sKey == "Level")
//			player.level = value.as<int>();
//		// etc...
//	}
//}

// Slightly faster way but unsafe
/*sol::table playerTable2 = F_Lua["player"];
player.name = playerTable2["Name"].get<std::string>();
player.level = playerTable2["Level"].get<int>();*/

// Calling Lua functions from C++
// 
// Unsafe, something might go wrong during execution
//float returnedValue = F_Lua["AddStuff"](3, 4);
//LogFloat(returnedValue, "Returned value unsafe: ");	

// Safely call Lua functions
//sol::protected_function awake = F_Lua["Awake"];
//if (awake)
//{
//	auto result = awake();
//	if (result.valid())
//	{
//		//LogFloat(result.get<float>(), "Returned value safe: ");
//	}
//	else
//	{
//		//sol::error err = result;
//		//LogString(err.what());						
//	}
//}

// Using std containers within Lua
//std::vector<GameObject> objects;
//// Create a Lua variable called allCharacters that holds a reference to characterVec in C++ side
//F_Lua["allObjects"] = &objects;
//
//LogInt((int)objects.size(), "Vector size before: ");
//F_Lua["CreateAllObjects"](15); // Call a Lua function to manipulate the new allCharacters variable
//LogInt((int)objects.size(), "Vector size after: ");
//
//for (int i = 0; i < objects.size(); i++)
//{
//	LogString("Name: " + objects[i].GetName());
//}