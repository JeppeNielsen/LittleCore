require "clion"

location "Build"

workspace "LittleCore"
   startproject "LittleEditor"

include "../Libs/Build_sokol.lua"
include "../Libs/Build_ShaderCompiler.lua"
include "../Libs/Build_LittleCore.lua"
include "../Libs/Build_ImGui.lua"

solution "LittleCore"
   configurations { "Release", "Debug" }
   if os.is64bit() and not os.istarget("windows") then
      platforms "ARM64"
   else
      platforms { "x86", "ARM64" }
   end
   filter "configurations:Release"
      defines
      {
         "NDEBUG",
         "XWIN_COCOA=1"
      }
      optimize "Full"
   filter "configurations:Debug*"
      defines
      {
         "_DEBUG",
         "XWIN_COCOA=1"
      }

      optimize "Debug"
      symbols "On"
   filter "platforms:x86"
      architecture "x86"
   filter "platforms:ARM64"
      architecture "ARM64"
   filter "system:macosx"
      xcodebuildsettings {
         ["MACOSX_DEPLOYMENT_TARGET"] = "11.0",
         ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
      };

project "LittleEditor"
   location("Build")

   kind "WindowedApp"
   language "C++"
   cppdialect "C++23"

   files { 
      "Source/**.cpp",
      "Source/**.hpp",
   }

   includedirs {
        "../../Engine/LittleCore/**",
        "../../Engine/ImGui",
        "../../External/sokol",
      "../../Engine/Sokol",
        "../../External/entt/single_include",
        "../../External/glm",
        "../../External/glaze/include",
        "../../External/imgui"
   }

   links { 
      "sokol",
      "LittleCore",
      "ShaderCompiler",
      "ImGui"
   }

   filter "system:windows"
      links { "gdi32", "kernel32", "psapi" }
   filter "system:linux"
      links { "dl", "GL", "pthread", "X11" }
   filter "system:macosx"
      links {
         "QuartzCore.framework", 
         "Cocoa.framework", 
         "IOKit.framework", 
         "Carbon.framework", 
         "CoreAudio.framework", 
         "AudioToolbox.framework",
         "Metal.framework",
         "MetalKit.framework",
         "OpenGL.framework"
      }
   SetClangPath()
