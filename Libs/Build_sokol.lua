local SOKOL_DIR = "../External/sokol"

project "sokol"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   cppdialect "C++23"
   exceptionhandling "Off"

   files
   {
      "../Engine/Sokol/sokol_impl.mm",
      path.join(SOKOL_DIR, "*.h"),
      path.join(SOKOL_DIR, "util/*.h"),
   }

   includedirs
   {
      path.join(SOKOL_DIR),
      path.join(SOKOL_DIR, "util"),
   }

   filter "system:windows"
      defines { "SOKOL_D3D11" }
   filter "system:macosx"
      defines { "SOKOL_METAL" }
   filter { "system:linux or system:bsd" }
      defines { "SOKOL_GLCORE" }
   filter {}
