require "clion"

location "Build"

workspace "LittleCore"
   startproject "TestJson"

include "../Libs/Build_bgfx.lua"
include "../Libs/Build_LittleCore.lua"

solution "LittleCore"
   configurations { "Release", "Debug" }
   if os.is64bit() and not os.istarget("windows") then
      platforms "x86_64"
   else
      platforms { "x86", "x86_64" }
   end
   filter "configurations:Release"
      defines
      {
         "NDEBUG",
         "BX_CONFIG_DEBUG=0",
         "XWIN_COCOA=1"
      }
      optimize "Full"
   filter "configurations:Debug*"
      defines
      {
         "_DEBUG",
         "BX_CONFIG_DEBUG=1",
         "XWIN_COCOA=1"
      }

      optimize "Debug"
      symbols "On"
   filter "platforms:x86"
      architecture "x86"
   filter "platforms:x86_64"
      architecture "x86_64"
   filter "system:macosx"
      xcodebuildsettings {
         ["MACOSX_DEPLOYMENT_TARGET"] = "10.9",
         ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
      };

project "TestJson"
   location("Build")

   kind "WindowedApp"
   language "C++"
   cppdialect "C++17"

   files { 
      "Source/*.cpp",
      "Source/**.hpp"
   }

   includedirs {
      "../../External/entt/single_include",
      "../../External/json/single_include",
      "../../Engine/LittleCore/**",
      "../../External/glm",
      "../../External/bgfx/include",
      "../../External/bx/include"
   }

   links { 
      "LittleCore",
      "bgfx",
      "bimg",
      "bx"
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
         "OpenGL.framework"
      }