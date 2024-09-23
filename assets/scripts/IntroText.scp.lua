-- IntroText


function Awake() 
     introText = GetObjectByName("Level1IntroText")
     if introText ~= NULL then
          introText:GetAnimation():Play("DisplayIntroText")
     end
end 

function Start() 

end 

function Update() 

end 

function AnimationEvent1()

end
