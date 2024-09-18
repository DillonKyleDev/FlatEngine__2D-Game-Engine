-- IntroText


function Awake() 
     introText = GetObjectByName("Level1IntroText")
     introText:GetAnimation():Play("DisplayLevel1Intro")
end 

function Start() 

end 

function Update() 

end 

function AnimationEvent1()

end
