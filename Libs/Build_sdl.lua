
local SDL_DIR = "../External/sdl"

project "SDL"
   kind "StaticLib"
   language "C"
   
   defines {
		"SDL_VIDEO_OPENGL_EGL=0",
		"SDL_VIDEO_OPENGL_CGL=0",
		"SDL_RENDER_DISABLED=1",
		"SDL_JOYSTICK_DISABLED=1",
		"SDL_HAPTIC_DISABLED=1",
		"SDL_SENSOR_DISABLED=1",
		"SDL_VIDEO_OPENGL=0",
		"SDL_VIDEO_OPENGL_ES2=0",
		"SDL_VIDEO_VULKAN=0"
   }

   buildoptions { "-x objective-c -fobjc-arc" }

   includedirs {
      path.join(SDL_DIR,"include"),
      path.join(SDL_DIR,"src"),
   }

   filter "system:windows"
      links { "gdi32", "kernel32", "psapi" }
   filter "system:linux"
      links { "dl", "GL", "pthread", "X11" }
   filter "system:macosx"
 		xcodebuildsettings {
 		   ["CLANG_ENABLE_OBJC_ARC"] = "YES",
 		   ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
 		   ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
      }
      files {
         path.join(SDL_DIR,"include/*.h"),

         path.join(SDL_DIR,"src/*.h"),
         path.join(SDL_DIR,"src/*.c"),

         path.join(SDL_DIR,"src/atomic/*.c"),

         path.join(SDL_DIR,"src/audio/*.h"),
         path.join(SDL_DIR,"src/audio/*.c"),
         path.join(SDL_DIR,"src/audio/coreaudio/*.h"),
         path.join(SDL_DIR,"src/audio/coreaudio/*.m"),
         path.join(SDL_DIR,"src/audio/disk/*.h"),
         path.join(SDL_DIR,"src/audio/disk/*.c"),
         path.join(SDL_DIR,"src/audio/dummy/*.h"),
         path.join(SDL_DIR,"src/audio/dummy/*.c"),

         path.join(SDL_DIR,"src/core/*.c"),

         path.join(SDL_DIR,"src/cpuinfo/*.c"),

         --path.join(SDL_DIR,"src/dynapi/*.c"),
         --path.join(SDL_DIR,"src/dynapi/*.h"),

         path.join(SDL_DIR,"src/events/*.h"),
         path.join(SDL_DIR,"src/events/*.c"),

         path.join(SDL_DIR,"src/file/*.c"),
         path.join(SDL_DIR,"src/file/cocoa/*.h"),
         path.join(SDL_DIR,"src/file/cocoa/*.m"),

         path.join(SDL_DIR,"src/filesystem/cocoa/*.h"),
         path.join(SDL_DIR,"src/filesystem/cocoa/*.m"),

         --path.join(SDL_DIR,"src/haptic/*.h"),
         --path.join(SDL_DIR,"src/haptic/*.c"),
         --path.join(SDL_DIR,"src/haptic/dummy/*.c"),
         --path.join(SDL_DIR,"src/haptic/darwin/*.h"),
         --path.join(SDL_DIR,"src/haptic/darwin/*.c"),

         --path.join(SDL_DIR,"src/joystick/*.h"),
         --path.join(SDL_DIR,"src/joystick/SDL_gamepad.c"),

         --path.join(SDL_DIR,"src/joystick/*.c"),
         --path.join(SDL_DIR,"src/joystick/dummy/*.c"),
         --path.join(SDL_DIR,"src/joystick/apple/*.h"),
         --path.join(SDL_DIR,"src/joystick/apple/*.m"),
         --path.join(SDL_DIR,"src/joystick/dummy/*.c"),

         path.join(SDL_DIR,"src/libm/*.c"),

         path.join(SDL_DIR,"src/locale/*.h"),
         path.join(SDL_DIR,"src/locale/macos/*.m"),

         path.join(SDL_DIR,"src/misc/*.h"),
         path.join(SDL_DIR,"src/misc/*.c"),
         path.join(SDL_DIR,"src/misc/macos/*.m"),

         path.join(SDL_DIR,"src/power/*.h"),
         path.join(SDL_DIR,"src/power/*.c"),
         path.join(SDL_DIR,"src/power/macos/*.c"),

         path.join(SDL_DIR,"src/render/*.h"),
         path.join(SDL_DIR,"src/render/*.c"),


         path.join(SDL_DIR,"src/stdlib/*.h"),
         path.join(SDL_DIR,"src/stdlib/*.c"),


         path.join(SDL_DIR,"src/thread/*.h"),
         path.join(SDL_DIR,"src/thread/*.c"),
         path.join(SDL_DIR,"src/thread/pthread/*.h"),
         path.join(SDL_DIR,"src/thread/pthread/*.c"),

         path.join(SDL_DIR,"src/timer/*.h"),
         path.join(SDL_DIR,"src/timer/*.c"),
         path.join(SDL_DIR,"src/timer/unix/*.c"),

         path.join(SDL_DIR,"src/video/*.h"),
         path.join(SDL_DIR,"src/video/*.c"),
         path.join(SDL_DIR,"src/video/cocoa/*.h"),
         path.join(SDL_DIR,"src/video/cocoa/*.m"),
         path.join(SDL_DIR,"src/video/dummy/*.h"),
         path.join(SDL_DIR,"src/video/dummy/*.c"),
         path.join(SDL_DIR,"src/video/yuv2rgb/*.h"),
         path.join(SDL_DIR,"src/video/yuv2rgb/*.c"),
      }

      removefiles { 
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoaopengl.h"),
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoaopengl.m"),
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoaopengles.h"),
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoaopengles.m"),
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoavulkan.h"),
         path.join(SDL_DIR,"src/video/cocoa/SDL_cocoavulkan.m"),
      }
