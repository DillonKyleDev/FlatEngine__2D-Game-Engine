-- MoveObject


 -- called on each script before Start() runs at the start of the gameloop (or upon instantiation) 
 function Awake() 
	 playerMappingContext = GetMappingContext("MC_Player")
 end 

 -- called at the start of the gameloop after Awake() (or upon instantiation) 
 function Start() 
	 --F_LogString(playerMappingContext:GetName())
	 LogString("Move Object called on ")
	animation = this_object:GetAnimation()
 end 

 -- called once per gameloop frame 
 function Update()	
	-- transform = this_object:GetTransform()
	--newPosition = Vector2:new()
	--newPosition:_y(transform:GetPosition():y() + 1)
	--transform:SetPosition(newPosition)
	-- rigidBody = this_object:GetRigidBody()
	-- force = Vector2:new(0, 10)
	
	-- --F_LogString(playerMappingContext:GetName())
	-- if playerMappingContext:GetName() == "MC_CharacterContext.json" then
	-- 	--F_LogString("Yes")
	-- end
	if playerMappingContext:Fired("IA_Jump") then
		LogString("Jumped")
		animation:Play()
	-- 	rigidBody:AddForce(force, 1)
	end
	


 end 
