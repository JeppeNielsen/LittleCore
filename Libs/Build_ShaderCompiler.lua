project "ShaderCompiler"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files {
      "../Engine/ShaderCompiler/**.hpp",
      "../Engine/ShaderCompiler/**.cpp",
      "../Engine/LittleCore/Files/FileHelper.cpp",

      -- Compile vendored sokol-shdc and dependencies directly in this project.
      -- No external sokol-tools build scripts are used.

      -- sokol-shdc frontend
      "../External/sokol-tools/src/shdc/**.cc",

      -- small utility dependencies
      "../External/sokol-tools/ext/getopt/src/getopt.c",
      "../External/sokol-tools/ext/pystring/pystring.cpp",
      "../External/sokol-tools/ext/fmt/src/format.cc",
      "../External/sokol-tools/ext/fmt/src/os.cc",

      -- SPIRV-Tools (subset used by sokol-shdc)
      "../External/sokol-tools/ext/SPIRV-Tools/source/*.cpp",
      "../External/sokol-tools/ext/SPIRV-Tools/source/util/*.cpp",
      "../External/sokol-tools/ext/SPIRV-Tools/source/val/*.cpp",
      "../External/sokol-tools/ext/SPIRV-Tools/source/opt/*.cpp",

      -- glslang (subset used by sokol-shdc)
      "../External/sokol-tools/ext/glslang/glslang/CInterface/*.cpp",
      "../External/sokol-tools/ext/glslang/glslang/GenericCodeGen/*.cpp",
      "../External/sokol-tools/ext/glslang/glslang/MachineIndependent/preprocessor/*.cpp",
      "../External/sokol-tools/ext/glslang/glslang/MachineIndependent/*.cpp",
      "../External/sokol-tools/ext/glslang/glslang/ResourceLimits/*.cpp",
      "../External/sokol-tools/ext/glslang/SPIRV/*.cpp",

      -- SPIRV-Cross
      "../External/sokol-tools/ext/SPIRV-Cross/*.cpp",

      -- Tint (for WGSL path in shdc sources)
      "../External/sokol-tools/ext/tint-extract/src/tint/**.cc",
   }

   removefiles {
      "../External/sokol-tools/src/shdc/main.cc",
      "../External/sokol-tools/ext/SPIRV-Cross/main.cpp",
      "../External/sokol-tools/ext/glslang/glslang/OSDependent/Windows/**.cpp",
      "../External/sokol-tools/ext/glslang/glslang/OSDependent/Unix/**.cpp",
      "../External/sokol-tools/ext/glslang/glslang/OSDependent/Web/**.cpp",
   }

   includedirs {
      "../Engine/ShaderCompiler",
      "../Engine/LittleCore/Files",
      "../Engine/Sokol",
      "../External/sokol",
      "../External/sokol-tools/src/shdc",
      "../External/sokol-tools/ext/getopt/include",
      "../External/sokol-tools/ext/pystring",
      "../External/sokol-tools/ext/fmt/include",
      "../External/sokol-tools/ext/SPIRV-Tools",
      "../External/sokol-tools/ext/SPIRV-Tools/include",
      "../External/sokol-tools/ext/SPIRV-Headers/include",
      "../External/sokol-tools/ext/generated",
      "../External/sokol-tools/ext/glslang",
      "../External/sokol-tools/ext/glslang/glslang",
      "../External/sokol-tools/ext/SPIRV-Cross",
      "../External/sokol-tools/ext/tint-extract",
      "../External/sokol-tools/ext/tint-extract/include",
   }

   defines {
      "FMT_UNICODE=0",
      "ENABLE_OPT=1",
      "TINT_BUILD_SPV_READER=1",
      "TINT_BUILD_WGSL_WRITER=1",
   }

   filter "system:windows"
      files {
         "../External/sokol-tools/ext/glslang/glslang/OSDependent/Windows/ossource.cpp",
      }
      defines {
         "GLSLANG_OSINCLUDE_WIN32=1",
      }

   filter "not system:windows"
      files {
         "../External/sokol-tools/ext/glslang/glslang/OSDependent/Unix/ossource.cpp",
      }
      defines {
         "GLSLANG_OSINCLUDE_UNIX=1",
      }

   filter {}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      cppdialect "C++23"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      cppdialect "C++23"
