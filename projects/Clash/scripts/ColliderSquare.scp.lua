-- ColliderSquare.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     ColliderSquare[my_id] =
     {
		-- Key value pairs here
     }
     local data = GetInstanceData("ColliderSquare", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("ColliderSquare", my_id)
     LogString("ColliderSquare : Start() called on "..this_object:GetName())
end

function Update()
     local data = GetInstanceData("ColliderSquare", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("ColliderSquare", my_id)
     LogString("Loading level1")
     LoadScene("Level1")
end

function OnBoxCollisionLeave(collidedWith)
end

function OnCircleCollision(collidedWith)
end

function OnCircleCollisionEnter(collidedWith)
end

function OnCircleCollisionLeave(collidedWith)
end

function OnButtonMouseOver()
end

function OnButtonMouseEnter()
end

function OnButtonMouseLeave()
end

function OnButtonLeftClick()
end

function OnButtonRightClick()
end