require "clion"

location "Build"

workspace "LittleCore"
   startproject "ScriptProject"

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
         ["MACOSX_DEPLOYMENT_TARGET"] = "11.0",
         ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
      };

project "ScriptProject"
   location("Build")

   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"

   files { 
      "ConsoleWindow/**.cpp",
      "ConsoleWindow/**.hpp",
      "Files/**.cpp",
      "Files/**.hpp",
      "../../../External/imgui/imgui.h",
      "../../../External/imgui/imgui.cpp",
      "../../../External/imgui/imgui_demo.cpp",
      "../../../External/imgui/imgui_draw.cpp",
      "../../../External/imgui/imgui_tables.cpp",
      "../../../External/imgui/imgui_widgets.cpp",
      "../../../External/imgui/misc/cpp/imgui_stdlib.cpp"
    }

   includedirs {
        "ConsoleWindow/",
        "Files/",
        "../../../External/imgui/",
        "../../../External/imgui/misc/cpp/",
        "../Source/ScriptsInclude/",
        "../../../External/entt/src"
   }

   links { 
      "SDL", 
      "bgfx",
      "bimg",
      "bx",
      "LittleCore",
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
         "OpenGL.framework"
      }