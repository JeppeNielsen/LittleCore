require "clion"

location "Build"

workspace "LittleCore"
   startproject "Engine"

include "../Libs/Build_sdl.lua"
include "../Libs/Build_bgfx.lua"
include "../Libs/Build_ShaderCompiler.lua"
include "../Libs/Build_LittleCore.lua"
include "../Libs/Build_ImGui.lua"
include "../Libs/Build_Netimgui.lua"
include "../Libs/Build_Application.lua"

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
   filter "platforms:ARM64"
      architecture "ARM64"
   filter "system:macosx"
      xcodebuildsettings {
         ["MACOSX_DEPLOYMENT_TARGET"] = "10.9",
         ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
      };

project "TestNetimguiClient"
   location("Build")

   kind "WindowedApp"
   language "C++"
   cppdialect "C++23"

   files { 
      "Source/**.cpp",
      "Source/**.hpp",
      "../../External/ImGuizmo/ImGuizmo.cpp"
   }

   includedirs {
      "Source/**",
      "../../External/netImgui/Code/Client",
      "../../Engine/LittleCore/**",
      "../../External/sdl/include",
      "../../External/bgfx/include",
      "../../External/bx/include",
      "../../External/entt/src",
      "../../Engine/ImGui",
      "../../External/imgui",
      "../../External/glm",
      "../../External/glaze/include",
      "../../Engine/Netimgui",
      "../../Engine/Application/**",
      "../../External/ImGuizmo"
   }

   links { 
      "SDL", 
      "bgfx",
      "bimg",
      "bx",
      "LittleCore",
      "ImGui",
      "ShaderCompiler",
      "Netimgui",
      "Application"
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

   SetClangPath()
   setBxCompat()

