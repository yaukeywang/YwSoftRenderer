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

    filter { "system:linux" }
        defines { "_LINUX" }

   filter { "system:macosx" }
        defines { "_MAC_OSX" }

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
        "libYw3d/Yw3d.h",
        "libYw3d/Yw3dBase.h",
        "libYw3d/Yw3dTypes.h",
        "libYw3d/YwBase.h",

        "libYw3d/Core/Yw3dBaseShader.h",
        "libYw3d/Core/Yw3dBaseShader.cpp",
        "libYw3d/Core/Yw3dBaseTexture.h",
        "libYw3d/Core/Yw3dBaseTexture.cpp",
        "libYw3d/Core/Yw3dCore.h",
        "libYw3d/Core/Yw3dCore.cpp",
        "libYw3d/Core/Yw3dCubeTexture.h",
        "libYw3d/Core/Yw3dCubeTexture.cpp",
        "libYw3d/Core/Yw3dDevice.h",
        "libYw3d/Core/Yw3dDevice.cpp",
        "libYw3d/Core/Yw3dIndexBuffer.h",
        "libYw3d/Core/Yw3dIndexBuffer.cpp",
        "libYw3d/Core/Yw3dPresentTarget.h",
        "libYw3d/Core/Yw3dPresentTarget.cpp",
        "libYw3d/Core/Yw3dPrimitiveAssembler.h",
        "libYw3d/Core/Yw3dRenderTarget.h",
        "libYw3d/Core/Yw3dRenderTarget.cpp",
        "libYw3d/Core/Yw3dShader.h",
        "libYw3d/Core/Yw3dShader.cpp",
        "libYw3d/Core/Yw3dSurface.h",
        "libYw3d/Core/Yw3dSurface.cpp",
        "libYw3d/Core/Yw3dTexture.h",
        "libYw3d/Core/Yw3dTexture.cpp",
        "libYw3d/Core/Yw3dVertexBuffer.h",
        "libYw3d/Core/Yw3dVertexBuffer.cpp",
        "libYw3d/Core/Yw3dVertexFormat.h",
        "libYw3d/Core/Yw3dVertexFormat.cpp",
        "libYw3d/Core/Yw3dVolume.h",
        "libYw3d/Core/Yw3dVolume.cpp",
        "libYw3d/Core/Yw3dVolumeTexture.h",
        "libYw3d/Core/Yw3dVolumeTexture.cpp",

        "libYw3d/Math/YwMath.h",
        "libYw3d/Math/YwMathMatrix.h",
        "libYw3d/Math/YwMathMatrix.inl",
        "libYw3d/Math/YwMathPlane.h",
        "libYw3d/Math/YwMathPlane.inl",
        "libYw3d/Math/YwMathQuaternion.h",
        "libYw3d/Math/YwMathQuaternion.inl",
        "libYw3d/Math/YwMathUtility.h",
        "libYw3d/Math/YwMathVector.h",
        "libYw3d/Math/YwMathVector.inl"
    }

    vpaths 
    {
        ["*"] = { "libYw3d/Yw*.h", "libYw3d/Yw*.inl", "libYw3d/Yw*.cpp" },
        ["Core"] = { "libYw3d/Core/Yw3d*.h", "libYw3d/Core/Yw3d*.inl", "libYw3d/Core/Yw3d*.cpp" },
        ["Math"] = { "libYw3d/Math/YwMath*.h", "libYw3d/Math/YwMath*.inl", "libYw3d/Math/YwMath*.cpp" }
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
        "libYw3d",
        "libYw3d/Core",
        "libYw3d/Math"
    }

    files
    { 
        --"libYwAppFramework/Yw*.h",
        --"libYwAppFramework/Yw*.cpp",

        "libYwAppFramework/Core/YwApplication.h",
        "libYwAppFramework/Core/YwApplication.cpp",
        "libYwAppFramework/Core/YwBaseApplication.h",
        "libYwAppFramework/Core/YwBaseApplication.cpp",
        "libYwAppFramework/Core/YwBaseInput.h",
        "libYwAppFramework/Core/YwCamera.h",
        "libYwAppFramework/Core/YwCamera.cpp",
        "libYwAppFramework/Core/YwEntity.h",
        "libYwAppFramework/Core/YwGraphics.h",
        "libYwAppFramework/Core/YwGraphics.cpp",
        "libYwAppFramework/Core/YwInput.h",
        "libYwAppFramework/Core/YwInput.cpp",
        "libYwAppFramework/Core/YwMesh.h",
        "libYwAppFramework/Core/YwMesh.cpp",
        "libYwAppFramework/Core/YwScene.h",
        "libYwAppFramework/Core/YwScene.cpp",
        "libYwAppFramework/Core/YwStateBlock.h",
        "libYwAppFramework/Core/YwStateBlock.cpp",

        "libYwAppFramework/IO/YwMeshLoader.h",
        "libYwAppFramework/IO/YwMeshLoaderObj.h",
        "libYwAppFramework/IO/YwMeshLoaderObj.cpp",

        --"libYwAppFramework/ThirdParty/*.h",
        --"libYwAppFramework/ThirdParty/*.cpp"
    }

    vpaths 
    {
        ["*"] = { "libYwAppFramework/Yw*.h", "libYwAppFramework/Yw*.inl", "libYwAppFramework/Yw*.cpp" },
        ["Core"] = { "libYwAppFramework/Core/Yw*.h", "libYwAppFramework/Core/Yw*.inl", "libYwAppFramework/Core/Yw*.cpp" },
        ["IO"] = { "libYwAppFramework/IO/Yw*.h", "libYwAppFramework/IO/Yw*.inl", "libYwAppFramework/IO/Yw*.cpp" },
        ["ThirdParty"] = { "libYwAppFramework/ThirdParty/*.h", "libYwAppFramework/ThirdParty/*.inl", "libYwAppFramework/ThirdParty/*.cpp" }
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

project "Demo1Triangle"
    language "C++"
    kind "WindowedApp"
    objdir (builddir .. "/Immediate")

    includedirs
    {
        "libYw3d",
        "libYw3d/Core",
        "libYw3d/Math",
        "libYwAppFramework",
        "libYwAppFramework/Core",
        "libYwAppFramework/IO",
        "libYwAppFramework/ThirdParty"
    }

    files
    { 
        "Demo1Triangle/YwDemoTriangle.h",
        "Demo1Triangle/YwDemoTriangle.cpp",
        "Demo1Triangle/YwDemoTriangleApp.h",
        "Demo1Triangle/YwDemoTriangleApp.cpp",
        "Demo1Triangle/YwDemoTriangleCamera.h",
        "Demo1Triangle/YwDemoTriangleCamera.cpp",
        "Demo1Triangle/YwDemoTriangleMain.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo1Triangle/Yw*.h", "Demo1Triangle/Yw*.inl", "Demo1Triangle/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    filter { "architecture:x86" }
        targetdir (builddir .. "/x86/Demo1Triangle")

    filter { "architecture:x86_64" }
        targetdir (builddir .. "/x64/Demo1Triangle")

    filter { "configurations:Debug*" }
        targetsuffix "D"
