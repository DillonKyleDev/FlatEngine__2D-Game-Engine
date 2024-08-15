#include "FlatEngine.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "ScriptComponent.h"


// Lua helpers + Functions that Lua can call
namespace FlatEngine
{
	sol::state F_Lua;

	void LuaTesting(GameObject& toSend)
	{
		// Load in lua script
		auto script = F_Lua.safe_script_file("../scripts/LuaScript.lua");
	
		F_Lua["this_object"] = toSend;
		if (script.valid())
		{
			sol::protected_function addSpriteFunc = F_Lua["AddSprite"];
			if (addSpriteFunc)
			{
				//auto player1 = 
				addSpriteFunc();
				//if (player1.valid())
				//{
				//	Transform* transform = player1.get<GameObject>().GetTransform();
				//}
				//else
				//{
				//	sol::error err = player1;
				//	LogString(err.what());
				//}
			}
		}
		else
		{
			sol::error err = script;
			LogString(err.what());
		}
	}

	void InitLua()
	{
		F_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
		RegisterLuaFunctions();
		RegisterLuaTypes();
	}

	// Inject functions that can be called from within Lua directly into the Lua state
	void RegisterLuaFunctions()
	{
		F_Lua["F_LogString"] = [](std::string line)
		{
			LogString(line, "[LUA]");
		};
		F_Lua["F_LogInt"] = [](int value, std::string line)
		{
			LogInt(value, line, "[LUA]");
		};
		F_Lua["F_LogFloat"] = [](float value, std::string line)
		{
			LogFloat(value, line, "[LUA]");
		};
		F_Lua["F_LogVector2"] = [](float xValue, float yValue, std::string line)
		{
			LogVector2(Vector2(xValue, yValue), line, "[LUA]");
		};
	}

	// Map C++ types to Lua "Types"
	void RegisterLuaTypes()
	{
		F_Lua.new_usertype<GameObject>("GameObject",
			sol::constructors<GameObject()>(),
			"GetName", &GameObject::GetName,
			"SetName", &GameObject::SetName,
			"IsActive", &GameObject::IsActive,
			"SetActive", &GameObject::SetActive,
			"GetID", &GameObject::GetID,
			"GetTransform", &GameObject::GetTransform,
			"AddSprite", &GameObject::AddSpriteComponent
			//"HasTag", &GameObject::HasTag,
			//"GetSprite", &GameObject::GetSprite,
			//"GetCamera", &GameObject::GetCamera,
			//"GetAnimation", &GameObject::GetAnimation,
			//"GetAudio", &GameObject::GetAudio,
			//"GetButton", &GameObject::GetButton,
			//"GetCanvas", &GameObject::GetCanvas,
			//"GetText", &GameObject::GetText,
			//"GetCharacterController", &GameObject::GetCharacterController,
			//"GetRigidBody", &GameObject::GetRigidBody,
			//"GetBoxCollider", &GameObject::GetBoxCollider,
			//"GetFirstChild", &GameObject::GetFirstChild,
			//"HasChildren", &GameObject::HasChildren,
			//"GetChildren", &GameObject::GetChildren,
		);
	}

	void RunAwakeAndStart()
	{
		for (std::pair<long, std::map<long, ScriptComponent>> object : GetLoadedScene()->GetScriptComponents())
		{
			for (auto &script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					std::string filepath = "../scripts/" + script.second.GetAttachedScript() + ".lua";

					auto script = F_Lua.safe_script_file(filepath);
					if (script.valid())
					{						
						F_Lua["this_object"] = &(*GetObjectById(object.first)); // Store this object inside the Lua state to be accessed by the next Lua function calls

						sol::protected_function awake = F_Lua["Awake"];
						if (awake.valid())
							awake();
						sol::protected_function start = F_Lua["Start"];
						if (start.valid())
							start();
					}
					else
					{
						sol::error err = script;
						LogString(err.what());
					}
				}
			}
		}
	}
}

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