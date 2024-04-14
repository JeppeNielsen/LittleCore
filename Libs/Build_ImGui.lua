project "ImGui"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/ImGui/**.hpp", 
      "../Engine/ImGui/**.cpp",
      "../Engine/ImGui/Backend/*.hpp",
      "../Engine/ImGui/Backend/*.h",
      "../Engine/ImGui/Backend/*.cpp",
      "../External/imgui/*.h",
      "../External/imgui/*.cpp",
      "../External/imgui/backends/imgui_impl_sdl3.h",
      "../External/imgui/backends/imgui_impl_sdl3.cpp"
   }

   includedirs {
      "../Engine/ImGui/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/sdl/include",
      "../External/imgui",
      "../External/imgui/backends"
   }

   sysincludedirs {
      "../Engine/ImGui/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/sdl/include",
      "../External/imgui"
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++17"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++17"

