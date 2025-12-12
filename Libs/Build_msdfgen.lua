project "msdfgen"
   kind "StaticLib" 
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { 
      "../External/msdfgen/core/**.cpp",
      "../External/msdfgen/ext/**.cpp",
   }

   includedirs {
      "../External/msdfgen",
      "../External/freetype/include",
   }

   externalincludedirs {
      "../External/msdfgen",
      "../External/freetype/include"
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"


   filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        links { "gdi32", "user32" }

    filter "system:linux"
        links { "m", "pthread" }

    filter "system:macosx"
        links { "m" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
