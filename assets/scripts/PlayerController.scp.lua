-- PlayerController


-- using Lua in FlatEngine: 

-- ### please visit RegisterLuaTypes() in "FlatEngine-Core/Source/LuaFunctions.cpp" to see the classes, class methods, and variables exposed to Lua scripts ### 
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
     --LogString("Start() called on "..this_object:GetName()) 
     mappingContext = GetMappingContext("MC_CharacterContext")
     characterController = this_object:GetCharacterController()
     rigidBody = this_object:GetRigidBody()
     i_maxJumps = 3
     i_totalJumps = 0
     --LogInt(3, "Here is an int: ");
     --LogString("Here is a string.");
end 

--called once per gameloop frame 
function Update() 
    if mappingContext:Fired("IA_Jump") then
        pendingXForces = rigidBody:GetPendingForces():x()
        rigidBody:SetPendingForces(Vector2:new(pendingXForces, 0));   
        rigidBody:AddForce(Vector2:new(0, 1), 20);
    end

    b_hasLanded = false
    b_movingLeft = false
    b_movingRight = false

    moveDirection = Vector2:new(0,0)

    if mappingContext:ActionPressed("IA_MoveRight") then
        b_movingRight = true
        moveDirection = Vector2:new(1,0)        
    end
    if mappingContext:ActionPressed("IA_MoveLeft") then
        b_movingLeft = true
        moveDirection = Vector2:new(-1,0)
    end

    if b_movingLeft and b_movingRight then
        moveDirection = Vector2:new(0,0)
    end
    
    if b_movingLeft or b_movingRight then
        characterController:MoveToward(moveDirection)
    end

    if mappingContext:Fired("IA_Fire") then
        iceShard = Instantiate("iceShard", Vector2:new(0,0))
        iceShard:GetRigidBody():AddForce(Vector2:new(1,0), 10)
        rigidBody:SetPendingForces(Vector2:new(pendingXForces, 0))   
        rigidBody:AddForce(Vector2:new(0, 1), 20);
    end
end 

function OnActiveCollision(collidedWith)
   -- LogString("OnActiveCollision called!")
end

function OnCollisionEnter(collidedWith)
    collidedName = collidedWith:GetParent():GetName()
    --LogString("Collision started with: " .. collidedName)
end

function OnCollisionLeave(collidedWith)
    --LogString("Collision ended with: " .. collidedWith:GetParent():GetName())
end