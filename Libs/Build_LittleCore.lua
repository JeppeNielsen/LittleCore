project "LittleCore"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/LittleCore/**.hpp", 
      "../Engine/LittleCore/**.cpp",
   }

   includedirs {
      "../Engine/LittleCore/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/single_include",
      "../External/glm",
      "../External/stb",
      "../External/sdl/include"
   }

   sysincludedirs {
      "../Engine/LittleCore/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/single_include",
      "../External/glm",
      "../External/stb",
      "../External/sdl/include"
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++17"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++17"

