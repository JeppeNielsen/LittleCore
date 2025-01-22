
local BGFX_DIR = "../External/bgfx"
local BIMG_DIR = "../External/bimg"
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

project "bgfx"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   cppdialect "C++14"
   exceptionhandling "Off"
   defines "__STDC_FORMAT_MACROS"

   files
   {
      path.join(BGFX_DIR, "include/bgfx/**.h"),
      path.join(BGFX_DIR, "src/*.cpp"),
      path.join(BGFX_DIR, "src/*.h"),
   }
   excludes
   {
      path.join(BGFX_DIR, "src/amalgamated.cpp"),
   }
   includedirs
   {
      path.join(BX_DIR, "include"),
      path.join(BIMG_DIR, "include"),
      path.join(BGFX_DIR, "include"),
      path.join(BGFX_DIR, "3rdparty"),
      path.join(BGFX_DIR, "3rdparty/khronos")
   }
   filter "action:vs*"
      defines "_CRT_SECURE_NO_WARNINGS"
      excludes
      {
         path.join(BGFX_DIR, "src/glcontext_glx.cpp"),
         path.join(BGFX_DIR, "src/glcontext_egl.cpp")
      }
   filter "system:macosx"
      files
      {
         path.join(BGFX_DIR, "src/*.mm"),
      }
   setBxCompat()

project "bimg"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   cppdialect "C++14"
   exceptionhandling "Off"
   files
   {
      path.join(BIMG_DIR, "include/bimg/*.h"),
      path.join(BIMG_DIR, "src/image.cpp"),
      path.join(BIMG_DIR, "src/image_gnf.cpp"),
      path.join(BIMG_DIR, "src/*.h"),
      path.join(BIMG_DIR, "3rdparty/astc-encoder/source/*.cpp")
   }
   includedirs
   {
      path.join(BX_DIR, "include"),
      path.join(BIMG_DIR, "include"),
      path.join(BIMG_DIR, "3rdparty/astc-encoder"),
      path.join(BIMG_DIR, "3rdparty/astc-encoder/include")
   }
   setBxCompat()

project "bx"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"
   cppdialect "C++14"
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
      