project "LittleCore"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/**.hpp", 
      "../Engine/**.cpp",
   }

   includedirs {
      "../Engine/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/single_include",
      "../External/glm",
   }

   sysincludedirs {
      "../Engine/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/entt/single_include",
      "../External/glm",
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++17"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++17"

