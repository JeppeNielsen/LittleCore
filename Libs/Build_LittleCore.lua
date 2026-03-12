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
      "../Engine/ShaderCompiler",
      "../External/sokol",
      "../Engine/Sokol",
      "../External/entt/src",
      "../External/glaze/include",
      "../External/glm",
      "../External/stb",
      "../External/hash-library",
      "../External/ufbx",
      "../External/msdfgen",
      "../External/freetype/include"
   }

   externalincludedirs {
      "../Engine/LittleCore/**",
      "../Engine/ShaderCompiler",
      "../External/sokol",
      "../Engine/Sokol",
      "../External/entt/src",
      "../External/glaze/include",
      "../External/glm",
      "../External/stb",
      "../External/hash-library",
      "../External/ufbx",
      "../External/msdfgen",
      "../External/freetype/include"
   }

   links {
      "sokol"
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"
