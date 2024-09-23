-- PlayerController.scp.lua


function Awake() 
    PlayerController[my_id] = 
    {
        gameScore = 0,
        scoreText = GetObjectByName("CurrentScore"),
        mappingContext = GetMappingContext("MC_Player"),
        characterController = this_object:GetCharacterController(),
        transform = this_object:GetTransform(),
        sprite = this_object:GetSprite(),
        rigidBody = this_object:GetRigidBody(),
        animation = this_object:GetAnimation(),
        boxCollider = this_object:GetBoxCollider(),
        healthBar = GetObjectByName("CurrentHealth"),
        shootTimer = 500,
        lastShotTime = 0,
        totalHealth = 100,
        currentHealth = 100
    }    
end 

function Start()     
    local data = PlayerController[my_id]
end 

function Update()     
    handleMovement();
    handleAttacks();    
end 

function UpdatePlayerHealthbar(id)
    local data = PlayerController[id]

    local healthSprite = data.healthBar:GetSprite()
    local spriteScale = healthSprite:GetScale()
    local healthScale = data.currentHealth / data.totalHealth
    healthSprite:SetScale(Vector2:new(healthScale, spriteScale:y()))
end

function DamagePlayer(id, amount)
    local data = PlayerController[id]    

    data.currentHealth = data.currentHealth - amount
    data.animation:Play("TakeDamage")
    UpdatePlayerHealthbar(id)

    if data.currentHealth <= 0 then
        data.boxCollider:SetActive(false)
        data.animation:Play("ShipExplode")     
    end
end

function handleAttacks()
    local data = PlayerController[my_id]
    
    if data.mappingContext:ActionPressed("IA_Shoot") and (GetTime() - data.lastShotTime > data.shootTimer) then
        data.lastShotTime = GetTime()
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

function AddToScore(amount)
    playerID = GetObjectByName("Player"):GetID()
    local data = PlayerController[playerID]

    data.gameScore = data.gameScore + amount
    data.scoreText:GetText():SetText(IntToString(data.gameScore)) 
end

function OnBoxCollision(collidedWith)    
end

function OnBoxCollisionEnter(collidedWith)
    local collidedName = collidedWith:GetParent():GetName()       
end

function OnBoxCollisionLeave(collidedWith)    
end