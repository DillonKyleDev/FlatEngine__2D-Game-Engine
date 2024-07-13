project "FlatEngine"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "../Vendor/includes/**", "**.h", "**.cpp" }

   includedirs
   {
      "Source"
   }

    externalincludedirs 
    { 
        "../Vendor/includes/ImGui/Backends",
        "../Vendor/includes/ImGui/ImGui_Docking",
        "../Vendor/includes/ImGui/ImGui_Docking/misc/debuggers",
        "../Vendor/includes/SDL2/include",
        "../Vendor/includes/SDL2/lib/x64",
        "../Vendor/includes/SDL2_Image/include",
        "../Vendor/includes/SDL2_Image/lib/x64",
        "../Vendor/includes/SDL2_Text/include",
        "../Vendor/includes/SDL2_Text/lib/x64",
        "../Vendor/includes/SDL2_Mixer/include",
        "../Vendor/includes/SDL2_Mixer/lib/x64",
        "../Vendor/includes/Json_Formatter",
        "../Vendor/includes/ImPlot",
        "../Vendor/includes/ImSequencer",
    }

    libdirs 
    {
         "../Vendor/includes/SDL2/lib/x64",
         "../Vendor/includes/SDL2_Image/lib/x64",
         "../Vendor/includes/SDL2_Text/lib/x64",
         "../Vendor/includes/SDL2_Mixer/lib/x64",
         "../Vendor/includes/ImPlot",
         "../Vendor/includes/ImSequencer",
    }
 
    links
    {
        "SDL2.lib",
        "SDL2main.lib",
        "SDL2_ttf.lib",
        "SDL2_image.lib",
        "d3d12.lib",
        "d3dcompiler.lib",
        "dxgi.lib",
        "SDL2_mixer.lib",
    }

   defines
   {
    "_WINDOWS"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "_DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "NDEBUG" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "NDEBUG" }
       runtime "Release"
       optimize "On"
       symbols "Off"