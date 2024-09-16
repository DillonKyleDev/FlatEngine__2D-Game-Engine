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
    mappingContext = GetMappingContext("MC_Player")
    characterController = this_object:GetCharacterController()
    transform = this_object:GetTransform()
    sprite = this_object:GetSprite()
    rigidBody = this_object:GetRigidBody()
end 

--called once per gameloop frame 
function Update()     
    handleMovement();
    handleAttacks();
end 

function handleAttacks()
    if mappingContext:Fired("IA_Shoot") then
        blasterRound = Instantiate("BlasterRound", transform:GetPosition())
        blasterRound:GetRigidBody():AddForce(Vector2:new(0,1), 10)
    end
end

function handleMovement()
    b_hasLanded = false
    b_movingLeft = false
    b_movingRight = false
    b_movingForward = false
    b_movingBackward = false

    moveDirection = Vector2:new(0,0)

    if mappingContext:ActionPressed("IA_MoveForward") then
        b_movingForward = true 
        moveDirection:_y(1)
    end

    if mappingContext:ActionPressed("IA_MoveBackward") then
        b_movingBackward = true
        moveDirection:_y(-1)
    end

    if mappingContext:ActionPressed("IA_MoveRight") then        
        b_movingRight = true
        moveDirection:_x(1)    
    end

    if mappingContext:ActionPressed("IA_MoveLeft") then
        b_movingLeft = true
        moveDirection:_x(-1)
    end

    if b_movingLeft and b_movingRight then
        moveDirection = Vector2:new(0, moveDirection:y())
    end
    if b_movingBackward and b_movingForward then
        moveDirection = Vector2:new(moveDirection:x(), 0)
    end 

    if b_movingLeft or b_movingRight or b_movingBackward or b_movingForward then
        characterController:MoveToward(moveDirection)

    end
    
    xVel = rigidBody:GetVelocity():x()
    if xVel < 0 then
        xVel = xVel * -1
    end
    LogFloat(xVel, "")

    sprite:SetScale(Vector2:new(1 - (xVel * 4), 1))
end

function OnBoxCollision(collidedWith)
    -- LogString("OnActiveCollision called!")
end

function OnBoxCollisionEnter(collidedWith)
    collidedName = collidedWith:GetParent():GetName()
    -- LogString("Collision started with: " .. collidedName)
end

function OnBoxCollisionLeave(collidedWith)
    --LogString("Collision ended with: " .. collidedWith:GetParent():GetName())
end