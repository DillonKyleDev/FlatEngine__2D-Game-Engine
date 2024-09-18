-- PlayerController.scp.lua


function Awake() 
    PlayerController[my_id] = 
    {
        mappingContext = GetMappingContext("MC_Player"),
        characterController = this_object:GetCharacterController(),
        transform = this_object:GetTransform(),
        sprite = this_object:GetSprite(),
        rigidBody = this_object:GetRigidBody()
    }        
end 

function Start()     
    local data = PlayerController[my_id]
end 

function Update()     
    handleMovement();
    handleAttacks();    
end 

function handleAttacks()
    local data = PlayerController[my_id]
    
    if data.mappingContext:Fired("IA_Shoot") then
        local blasterRound = Instantiate("BlasterRound", data.transform:GetPosition())
        blasterRound:GetRigidBody():AddForce(Vector2:new(0,1), 10)
    end
end

function handleMovement()
    local data = PlayerController[my_id]

    local b_hasLanded = false
    local b_movingLeft = false
    local b_movingRight = false
    local b_movingForward = false
    local b_movingBackward = false

    local moveDirection = Vector2:new(0,0)

    if data.mappingContext:ActionPressed("IA_MoveForward") then
        b_movingForward = true 
        moveDirection:SetY(1)
    end

    if data.mappingContext:ActionPressed("IA_MoveBackward") then
        b_movingBackward = true
        moveDirection:SetY(-1)
    end

    if data.mappingContext:ActionPressed("IA_MoveRight") then        
        b_movingRight = true
        moveDirection:SetX(1)    
    end

    if data.mappingContext:ActionPressed("IA_MoveLeft") then
        b_movingLeft = true
        moveDirection:SetX(-1)
    end

    if b_movingLeft and b_movingRight then
        moveDirection = Vector2:new(0, moveDirection:y())
    end
    if b_movingBackward and b_movingForward then
        moveDirection = Vector2:new(moveDirection:x(), 0)
    end 

    if b_movingLeft or b_movingRight or b_movingBackward or b_movingForward then
        data.characterController:MoveToward(moveDirection)

    end
    
    local xVel = data.rigidBody:GetVelocity():x()
    if xVel < 0 then
        xVel = xVel * -1
    end

    data.sprite:SetScale(Vector2:new(1 - (xVel * 4), 1))
end

function OnBoxCollision(collidedWith)    
end

function OnBoxCollisionEnter(collidedWith)
    local collidedName = collidedWith:GetParent():GetName()   
    LogString("From Player") 
end

function OnBoxCollisionLeave(collidedWith)    
end