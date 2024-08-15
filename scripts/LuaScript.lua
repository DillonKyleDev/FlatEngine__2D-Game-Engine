-- Lua Testing

function MoveTransform(transform)
	transform.SetPosition(10,10);
end;

function AddSprite()
	F_LogString("In AddSprite");
	F_LogString(this_object:GetName());
	this_object:AddSprite(-1, true, false);	
end


function Awake()
	print("[LUA] Awake called");	
	F_LogString("Awake() called on "..this_object:GetName());
end

function Start()
	print("[LUA] Start called");
	F_LogString("Start() called on "..this_object:GetName());
end

function Update()
	print("[LUA] Update called");
	F_LogString("Update() called on "..this_object:GetName());
end