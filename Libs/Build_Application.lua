project "Application"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   
   files { 
      "../Engine/Application/**.hpp",
      "../Engine/Application/**.cpp",
      "../External/ImGuizmo/ImGuizmo.cpp"
   }

   includedirs {
        "../Engine/Application/**",
        "../Engine/LittleCore/**",
        "../Engine/ImGui/",
        "../External/sdl/include",
        "../External/bgfx/include",
        "../External/bx/include",
        "../External/entt/src",
        "../External/glaze/include",
        "../External/glm",
        "../External/stb",
        "../External/sdl/include",
        "../External/hash-library",
        "../External/ufbx",
        "../External/msdfgen",
        "../External/freetype/include",
        "../External/imgui",
        "../External/ImGuizmo",
        "../Engine/Netimgui"
   }

   externalincludedirs {

    }

  links {
    "LittleCore",
    "ImGui",
    "SDL",
    "bgfx",
    "bimg",
    "bx",
    "ImGui",
    "ShaderCompiler",
    "Netimgui",
    "Application"
  }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"