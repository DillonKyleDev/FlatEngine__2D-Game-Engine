-- IntroText


function Awake() 
     introText = GetObjectByName("Level1IntroText")
     introText:GetAnimation():Play("DisplayIntroText")
end 

function Start() 

end 

function Update() 

end 

function AnimationEvent1()

end
