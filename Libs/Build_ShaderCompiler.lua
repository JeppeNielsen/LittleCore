project "ShaderCompiler"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files {
      "../Engine/ShaderCompiler/**.hpp",
      "../Engine/ShaderCompiler/**.cpp",
      "../Engine/LittleCore/Files/FileHelper.cpp",
   }

   includedirs {
      "../Engine/ShaderCompiler",
      "../Engine/LittleCore/Files",
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"
