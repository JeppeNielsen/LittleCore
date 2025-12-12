project "freetype"
    kind "StaticLib"
    language "C"
    targetdir "../bin/%{cfg.buildcfg}"

    -- Apply to ALL files in this project
    defines {
        "FT2_BUILD_LIBRARY"
    }

    files {
        "../External/freetype/src/**.c",
        "../External/freetype/src/**.h",
        "../External/freetype/include/**.h"
    }

    includedirs {
        "../External/freetype/include",
        "../External/freetype/src"
    }

    removefiles {
      "../External/freetype/src/tools/**.c",
      "../External/freetype/src/tests/**.c",
      --"../External/freetype/src/gxvalid/**.c",
      --"../External/freetype/src/lzw/**.c",
      --"../External/freetype/src/psaux/**.c",
      --"../External/freetype/src/otvalid/**.c",  -- optional OpenType validator
      --"../External/freetype/src/pshinter/**.c",  -- optional OpenType validator
      
      --"../External/freetype/src/gzip/**.c",   -- <— fixes ft_memcpy errors
      --"../External/freetype/src/bzip2/**.c",  -- optional
      --"../External/freetype/src/lzw/**.c",    -- optional
      --"../External/freetype/src/pfr/**.c",    -- optional legacy formats
      --"../External/freetype/src/type42/**.c", -- optional
      --"../External/freetype/src/winfonts/**.c", -- optional
      --"../External/freetype/src/sfwoff/**.c",  -- WOFF
      --"../External/freetype/src/pcf/**.c",     -- PCF bitmap fonts
      --"../External/freetype/src/svg/**.c",     -- SVG OpenType
      --"../External/freetype/src/gzip/**.c",
      --"../External/freetype/src/sfnt/**.c",
   }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        runtime "Release"

    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        links { "gdi32", "user32" }

    filter "system:linux"
        links { "m", "pthread" }

    filter "system:macosx"
        links { "m" }
        --buildoptions { "-include", path.getabsolute("../External/freetype/include/ft2build.h") }
        buildoptions { "-include " .. path.getabsolute("../External/freetype/include/freetype/config/ftstdlib.h") }
        --.. " -include " .. path.getabsolute("/Users/jeppe/Jeppes/LittleCore/External/freetype/src/raster/ftmisc.h") }


        filter {}
