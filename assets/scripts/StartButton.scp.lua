-- StartButton


-- using Lua in FlatEngine: 

-- ### please visit RegisterLuaTypes()     in "FlatEngine-Core/Source/LuaFunctions.cpp" to see the classes, class methods, and variables exposed to Lua scripts ### 
-- ### please visit RegisterLuaFunctions() in "FlatEngine-Core/Source/LuaFunctions.cpp" to see the FlatEngine functions exposed to Lua scripts              ### 

-- use "this_object" to reference the object that is attached to this script 
-- use ":" to access member variables and functions of objects: object:member_variable ..or.. object::member_function() 
-- to concatinate two or more strings, use two periods: ".."  LogString("Just add two periods between arguments like"..string_variable_name) 
-- to create new objects of type Type with construction parameters, use: Type:new(parameters,...)  


-- called on each script before Start() runs at the start of the gameloop (or upon instantiation) 
function Awake() 

end 

-- called at the start of the gameloop after Awake() (or upon instantiation) 
function Start() 
     LogString("Start() called on "..this_object:GetName()) 

end 
--called once per gameloop frame 
function Update() 

end 

function OnButtonLeftClick()
     LoadScene("Level1")
end

function OnButtonMouseOver()
     -- LogString("Mouse over")
 end
 
 function OnButtonRightClick()
     -- LogString("Right Click!")
 end