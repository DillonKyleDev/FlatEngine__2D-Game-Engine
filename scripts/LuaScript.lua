
a = "Hello from script 2"
b = 5

function AddStuff(a, b)
	print("[LUA] AddStuff("..a..", "..b..") called");
	return a + b;
end

function DoAThing(a, b)
	print("[LUA] DoAThing("..a..", "..b..") called");

	c = HostFunction(a + 5, b + 5);

	print("[LUA] Returned "..c.."");
	return c;
end


function MoveTransform(transform)
	transform.SetPosition(10,10);
end;


function Awake()
	print("[LUA] Awake called");

	F_LogString("Awake()");
end


function CreateGameObject()
	local p = GameObject:new();
	-- p.SetName("Dillon");
	--local char = LuaCharacter:new();
	--F_LogString(char.GetName());
	return p;
end

function CreateAllObjects(objects)
	allObjects:clear();

	for i=0, objects do
		
		local p = GameObject:new();
		p:SetName("NewObject");
		F_LogString(p:GetName());		

		allObjects:add(p);
	end
end

function Start()
	print("[LUA] Start called");

	F_LogString("Start()");

	a = F_NewArray(1000);
	print(a);
	print(F_GetArraySize(a));
	
	for i=1, 1000 do
		F_SetArray(a, i, 1/i);
	end

	print(F_GetArray(a, 10));  --> 0.1	
end


function Update()
	print("[LUA] Update called");

	F_LogString("Update()");
end