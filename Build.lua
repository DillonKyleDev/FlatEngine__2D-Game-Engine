-- premake5.lua
workspace "FlatEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "FlatEngine-Hub"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"


group "Core"
	include "FlatEngine-Core/Build-Core.lua"
group ""

include "FlatEngine-Editor/Build-Editor.lua"
include "FlatEngine-Runtime/Build-Runtime.lua"
include "FlatEngine-Hub/Build-Hub.lua"