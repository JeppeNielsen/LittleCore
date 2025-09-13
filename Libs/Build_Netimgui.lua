project "Netimgui"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/Netimgui/**.hpp", 
      "../Engine/Netimgui/**.cpp",
      "../External/imgui/*.h",
      "../External/imgui/*.cpp",
      "../External/netImgui/Code/ServerApp/Source/*.cpp",
      "../External/netImgui/Code/Client/Private/*.cpp"
   }

   includedirs {
      "../Engine/ImGui",
      "../Engine/Netimgui",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/sdl/include",
      "../External/imgui",
      "../External/imgui/backends",
      "../External/netImgui/Code/ServerApp/Source/",
      "../External/netImgui/Code/Client/",
      "../External/netImgui/Code/"
   }

   externalincludedirs {
      "../Engine/ImGui/**",
      "../Engine/Netimgui/**",
      "../External/sdl/include",
      "../External/bgfx/include",
      "../External/bx/include",
      "../External/sdl/include",
      "../External/imgui"
   }

   links { 
      "ImGui"
   }
  
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"