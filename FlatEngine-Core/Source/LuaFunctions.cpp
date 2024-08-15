#include "FlatEngine.h"
#include "GameObject.h"
#include "Scene.h"


// Lua helpers + Functions that Lua can call
namespace FlatEngine
{
	sol::state F_Lua;

	void InitLua()
	{
		F_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
		RegisterLuaFunctions();
		RegisterLuaTypes();

		// Load in lua script
		auto script = F_Lua.safe_script_file("../scripts/LuaScript.lua");
		if (script.valid())
		{
			sol::protected_function createObject = F_Lua["CreateGameObject"];
			if (createObject)
			{
				auto player1 = createObject();
				if (player1.valid())
				{
					//LogString(player1.get<GameObject>().GetName());
				}
				else
				{
					sol::error err = player1;
					LogString(err.what());
				}
			}

			// Using std containers within Lua
			std::vector<GameObject> objects;
			// Create a Lua variable called allCharacters that holds a reference to characterVec in C++ side
			F_Lua["allObjects"] = &objects;

			LogInt((int)objects.size(), "Vector size before: ");
			F_Lua["CreateAllObjects"](15); // Call a Lua function to manipulate the new allCharacters variable
			LogInt((int)objects.size(), "Vector size after: ");

			for (int i = 0; i < objects.size(); i++)
			{
				LogString("Name: " + objects[i].GetName());
			}
		}
		else
		{
			sol::error err = script;
			LogString(err.what());
		}
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

		F_Lua["F_GetOwner"] = []()
			{

			};
	}

	// Map C++ types to Lua "Types"
	void RegisterLuaTypes()
	{
		struct Character
		{
			std::string name = "Test";
			std::string title;
			std::string family;
			int level;
			std::string GetName() { return name; };
		};
		Character player;

		F_Lua.new_usertype<Character>("LuaCharacter",
			sol::constructors<Character()>(),
			"name", sol::as_function(&Character::name),
			"family", sol::property(&Character::family),
			"title", sol::property(&Character::title),
			"level", sol::property(&Character::level)
		);

		F_Lua.new_usertype<GameObject>("GameObject",
			sol::constructors<GameObject()>(),
			//"name", sol::as_function(&GameObject::name)
			"name", sol::property(&GameObject::GetName, &GameObject::SetName),
			"luaName", sol::as_function(&GameObject::luaName),
			"GetName", &GameObject::GetName,
			"SetName", &GameObject::SetName
			//"{name}", &type::member_variable
		);
	}

	void RunAwakeAndStart()
	{
		for (std::pair<long, std::vector<std::string>> object : GetLoadedScene()->GetLuaScriptsByOwner())
		{
			for (std::string scriptName : object.second)
			{
				std::string filepath = "../scripts/" + scriptName + ".lua";		

				auto script = F_Lua.safe_script_file(filepath);
				if (script.valid())
				{
					LogString("Script file loaded.");

					// Safely call Lua functions
					sol::protected_function addStuffFunc = F_Lua["AddStuff"];
					if (addStuffFunc)
					{
						auto returnedValueSafe = addStuffFunc(3, 4);
						if (returnedValueSafe.valid())
						{
							LogFloat(returnedValueSafe.get<float>(), "Returned value safe: ");
						}
						else
						{
							sol::error err = returnedValueSafe;
							LogString(err.what());						
						}
					}
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