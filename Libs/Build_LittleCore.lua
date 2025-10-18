include "Clang.lua"

project "LittleCore"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/LittleCore/**.hpp", 
      "../Engine/LittleCore/**.cpp",
      "../External/hash-library/**.cpp",
      "../External/ufbx/*.c",
   }

   includedirs {
      "../Engine/LittleCore/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/src",
      "../External/glaze/include",
      "../External/glm",
      "../External/stb",
      "../External/sdl/include",
      "../External/hash-library",
      "../External/ufbx"
   }

   externalincludedirs {
      "../Engine/LittleCore/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/src",
      "../External/glaze/include",
      "../External/glm",
      "../External/stb",
      "../External/sdl/include",
      "../External/hash-library",
      "../External/ufbx"
   }

   links {
      "bgfx"
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"
