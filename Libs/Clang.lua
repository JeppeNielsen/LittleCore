local CLANG_PATH = "../../Scripting/clang18/lib"
local CLANG_PATH_ABSOLUTE = path.getabsolute(CLANG_PATH)

function SetClangPath()
   filter "system:macosx"
      buildoptions { "-arch arm64" }
      linkoptions { "-arch arm64 -L"..CLANG_PATH_ABSOLUTE.." -rpath "..CLANG_PATH_ABSOLUTE.."" }
end




