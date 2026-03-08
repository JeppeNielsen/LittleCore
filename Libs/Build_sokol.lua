local SOKOL_DIR = "../External/sokol"
local BX_DIR = "../External/bx"

function setBxCompat()
   filter "action:vs*"
      includedirs { path.join(BX_DIR, "include/compat/msvc") }
   filter { "system:windows", "action:gmake" }
      includedirs { path.join(BX_DIR, "include/compat/mingw") }
   filter { "system:macosx" }
      includedirs { path.join(BX_DIR, "include/compat/osx") }
      buildoptions { "-x objective-c++" }
end

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

project "bx"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   cppdialect "C++23"
   exceptionhandling "Off"
   defines "__STDC_FORMAT_MACROS"
   files
   {
      path.join(BX_DIR, "include/bx/*.h"),
      path.join(BX_DIR, "include/bx/inline/*.inl"),
      path.join(BX_DIR, "src/*.cpp")
   }
   excludes
   {
      path.join(BX_DIR, "src/amalgamated.cpp"),
      path.join(BX_DIR, "src/crtnone.cpp")
   }
   includedirs
   {
      path.join(BX_DIR, "3rdparty"),
      path.join(BX_DIR, "include")
   }
   filter "configurations:Release"
      defines "BX_CONFIG_DEBUG=0"
   filter "configurations:Debug"
      defines "BX_CONFIG_DEBUG=1"
   filter "action:vs*"
      defines "_CRT_SECURE_NO_WARNINGS"
      defines "_CRT_SECURE_NO_WARNINGS"

   setBxCompat()
