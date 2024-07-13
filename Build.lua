-- premake5.lua
workspace "FlatEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "FlatEngine"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "FlatEngine"
	include "WindowsApplication/Build-Core.lua"
group ""

include "WindowsApplication/Build-Core.lua"