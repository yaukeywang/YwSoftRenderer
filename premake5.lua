-- Add by yaukey at 2018-06-30.
-- premake5 file to build YwSoftRenderer.
-- http://premake.github.io/
--

local action = _ACTION or ""
local todir = "Workspace/" .. action
local builddir = "Build"

solution "YwSoftRenderer"
    location (todir)

    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }

    filter { "platforms:x86" }
        architecture "x86"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "system:windows" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter { "configurations:Debug*" }
        defines { "DEBUG" }

    filter { "configurations:Release*" }
        defines { "NDEBUG" }
        optimize "On"

    filter { "architecture:x86" }
        targetdir (builddir .. "/x86")

    filter { "architecture:x86_64" }
        targetdir (builddir .. "/x64")

project "libYw3d"
    language "C++"
    kind "StaticLib"
    objdir (builddir .. "/Immediate")

    includedirs
    {
    }

    files
    { 
        "libYw3d/Yw*.h",
        "libYw3d/Yw*.cpp",
        "libYw3d/Core/Yw3d*.h",
        "libYw3d/Core/Yw3d*.cpp",
        "libYw3d/Math/YwMath*.h",
        "libYw3d/Math/YwMath*.cpp"
    }

    vpaths 
    {
        ["*"] = { "libYw3d/Yw*.h", "libYw3d/Yw*.cpp" },
        ["Core"] = { "libYw3d/Core/Yw3d*.h", "libYw3d/Core/Yw3d*.cpp" },
        ["Math"] = { "libYw3d/Math/YwMath*.h", "libYw3d/Math/YwMath*.cpp" }
    }

    filter { "architecture:x86" }
        targetdir (builddir .. "/x86/libYw3d")

    filter { "architecture:x86_64" }
        targetdir (builddir .. "/x64/libYw3d")

    filter { "configurations:Debug*" }
        targetsuffix "D"

    -- filter { "system:windows" }
    --     postbuildcommands
    --     {
    --         '{MKDIR} "%{wks.location}../../Build/Bin/%{cfg.platform}/%{prj.name}"',
    --         '{COPY} "%{cfg.targetdir}/%{cfg.buildtarget.name}" "%{wks.location}../../' .. builddir .. '/Bin/%{cfg.platform}/%{prj.name}"'
    --     }

project "libYwAppFramework"
    language "C++"
    kind "StaticLib"
    objdir (builddir .. "/Immediate")

    includedirs
    {
        "libYw3d"
    }

    files
    { 
        "libYwAppFramework/Yw*.h",
        "libYwAppFramework/Yw*.cpp",
        "libYwAppFramework/Core/Yw*.h",
        "libYwAppFramework/Core/Yw*.cpp",
        "libYwAppFramework/ThirdParty/*.h",
        "libYwAppFramework/ThirdParty/*.cpp"
    }

    vpaths 
    {
        ["*"] = { "libYwAppFramework/Yw*.h", "libYwAppFramework/Yw*.cpp" },
        ["Core"] = { "libYwAppFramework/Core/Yw*.h", "libYwAppFramework/Core/Yw*.cpp" },
        ["ThirdParty"] = { "libYwAppFramework/ThirdParty/*.h", "libYwAppFramework/ThirdParty/*.cpp" }
    }

    links
    {
        "libYw3d"
    }

    filter { "architecture:x86" }
        targetdir (builddir .. "/x86/libYwAppFramework")

    filter { "architecture:x86_64" }
        targetdir (builddir .. "/x64/libYwAppFramework")

    filter { "configurations:Debug*" }
        targetsuffix "D"
