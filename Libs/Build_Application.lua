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
        "../External/sokol",
      "../Engine/Sokol",
        "../External/bx/include",
        "../External/entt/src",
        "../External/glaze/include",
        "../External/glm",
        "../External/stb",
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
    "sokol",
    "bx",
    "ImGui",
    "ShaderCompiler",
    "Netimgui",
  }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"
