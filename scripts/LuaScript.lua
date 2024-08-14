player = { Title = "Squire", Name = "Dillon", Family = "Wirral", Level = 20 }


function AddStuff(a, b)
	print("[LUA] AddStuff("..a..", "..b..") called")
	return a + b
end

function DoAThing(a, b)
	print("[LUA] DoAThing("..a..", "..b..") called")

	c = HostFunction(a + 5, b + 5) 

	print("[LUA] Returned "..c.."")
	return c
end


function Awaker()
end


function Start()
end


function Update()
end