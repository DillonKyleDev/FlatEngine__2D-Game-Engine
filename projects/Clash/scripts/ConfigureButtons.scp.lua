-- ConfigureButtons.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     ConfigureButtons[my_id] =
     {
          context = GetMappingContext("MC_Player"),
		canvas = GetObjectByName("RemapButtons"),
          keyCodeTexts = {}
     }
     local data = GetInstanceData("ConfigureButtons", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("ConfigureButtons", my_id)

     local inputMappings = data.context:GetInputMappings()

     local offset = 0

     for i, inputMapping in pairs(inputMappings) do 

          local keyCode = inputMapping:KeyCode()
          local inputAction = inputMapping:InputActionName()
          
          local mapping = CreateGameObject(data.canvas:GetID())
          mapping:SetName(inputAction)
          local mappingTransform = mapping:GetTransform()
          local mappingPosition = mappingTransform:GetPosition()
          mappingTransform:SetPosition(Vector2:new(mappingPosition:x(), mappingPosition:y() + offset))

          local label = CreateGameObject(mapping:GetID())
          local labelText = label:AddText()
          labelText:SetPivotPoint("PivotLeft")
          labelText:SetText(inputAction)
          local labelTransform = label:GetTransform()
          local labelPosition = labelTransform:GetPosition()
          labelTransform:SetPosition(Vector2:new(4, labelPosition:y()))

          local button = mapping:AddButton()
          local mappingText = mapping:AddText()

          data.keyCodeTexts[i] = mappingText

          mappingText:SetText(keyCode)
          button:SetActiveDimensions(7, 1)

          functionParams = ParameterList:new()
          contextName = EventParameter:new()
          inputActionName = EventParameter:new()
          timeoutTime = EventParameter:new()

          contextName:SetType("string")
          contextName:SetString("MC_Player")
          inputActionName:SetType("string")
          inputActionName:SetString(inputAction)
          timeoutTime:SetType("int")
          timeoutTime:SetInt(4000)
          functionParams:AddParameter(contextName)
          functionParams:AddParameter(inputActionName)
          functionParams:AddParameter(timeoutTime)

          button:SetLuaFunctionName("RemapInputAction")
          button:SetLuaFunctionParams(functionParams)

          offset = offset + 1
     end
end

function RemapInputAction(contextName, inputActionName, timeoutTime)
     Remap(contextName:string(), inputActionName:string(), timeoutTime:int())
end

function Update()
     local data = GetInstanceData("ConfigureButtons", my_id)

     local inputMappings = data.context:GetInputMappings()

     for i, inputMapping in pairs(inputMappings) do 
          data.keyCodeTexts[i]:SetText(inputMapping:KeyCode())
     end
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("ConfigureButtons", my_id)
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