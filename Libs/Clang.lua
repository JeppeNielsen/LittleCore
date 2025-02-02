local CLANG_PATH = "../../Scripting/clang18/lib"

function SetClangPath()
   filter "system:macosx"
      buildoptions { "-arch arm64" }
      linkoptions { "-arch arm64 -L"..CLANG_PATH.." -rpath "..CLANG_PATH.."" }
end




