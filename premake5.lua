-- Add by yaukey at 2018-06-30.
-- premake5 file to build YwSoftRenderer.
-- http://premake.github.io/
--

local action = _ACTION or ""
local todir = "Workspace/" .. action
local builddir = "Build"
local appbuilddir = builddir .. "/YwSoftRenderer"
local appresdir = appbuilddir .. "/Resources"
local abssrcdatadir = "%{wks.location}../../Resources"
local absdstdatadir = "%{wks.location}../../" .. appresdir

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
        "libYw3d",
        "libYw3d/Core",
        "libYw3d/Math"
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

project "libYwAppFramework"
    language "C++"
    kind "StaticLib"
    objdir (builddir .. "/Immediate")

    includedirs
    {
        "libYw3d",
        "libYw3d/Core",
        "libYw3d/Math",
        "libYwAppFramework",
        "libYwAppFramework/Core",
        "libYwAppFramework/IO",
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
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
        "libYwAppFramework/Core/YwModel.h",
        "libYwAppFramework/Core/YwModel.cpp",
        "libYwAppFramework/Core/YwScene.h",
        "libYwAppFramework/Core/YwScene.cpp",
        "libYwAppFramework/Core/YwStateBlock.h",
        "libYwAppFramework/Core/YwStateBlock.cpp",

        "libYwAppFramework/IO/YwFileIO.h",
        "libYwAppFramework/IO/YwFileIO.cpp",
        "libYwAppFramework/IO/YwModelLoader.h",
        "libYwAppFramework/IO/YwModelLoader.cpp",
        "libYwAppFramework/IO/YwModelLoaderWavefrontObj.h",
        "libYwAppFramework/IO/YwModelLoaderWavefrontObj.cpp",
        "libYwAppFramework/IO/YwTextureLoader.h",
        "libYwAppFramework/IO/YwTextureLoader.cpp",
        "libYwAppFramework/IO/YwTextureLoaderBmp.h",
        "libYwAppFramework/IO/YwTextureLoaderBmp.cpp",
        "libYwAppFramework/IO/YwTextureLoaderPng.h",
        "libYwAppFramework/IO/YwTextureLoaderPng.cpp",
        "libYwAppFramework/IO/YwTextureLoaderTga.h",
        "libYwAppFramework/IO/YwTextureLoaderTga.cpp",

        "libYwAppFramework/Resource/YwResourceManager.h",
        "libYwAppFramework/Resource/YwResourceManager.cpp",

        "libYwAppFramework/ThirdParty/libpng/*.h",
        "libYwAppFramework/ThirdParty/libpng/*.c",
        "libYwAppFramework/ThirdParty/libpng/*.cpp",
        "libYwAppFramework/ThirdParty/libtarga/*.h",
        "libYwAppFramework/ThirdParty/libtarga/*.c",
        "libYwAppFramework/ThirdParty/libtarga/*.cpp",
        "libYwAppFramework/ThirdParty/zlib/*.h",
        "libYwAppFramework/ThirdParty/zlib/*.c",
        "libYwAppFramework/ThirdParty/zlib/*.cpp",

        "libYwAppFramework/Utility/*.h",
        "libYwAppFramework/Utility/*.cpp"
    }

    vpaths 
    {
        ["*"] = { "libYwAppFramework/Yw*.h", "libYwAppFramework/Yw*.inl", "libYwAppFramework/Yw*.cpp" },
        ["Core"] = { "libYwAppFramework/Core/Yw*.h", "libYwAppFramework/Core/Yw*.inl", "libYwAppFramework/Core/Yw*.cpp" },
        ["IO"] = { "libYwAppFramework/IO/Yw*.h", "libYwAppFramework/IO/Yw*.inl", "libYwAppFramework/IO/Yw*.cpp" },
        ["Resource"] = { "libYwAppFramework/Resource/Yw*.h", "libYwAppFramework/Resource/Yw*.inl", "libYwAppFramework/Resource/Yw*.cpp" },
        ["ThirdParty"] = { "libYwAppFramework/ThirdParty/*.h", "libYwAppFramework/ThirdParty/*.inl", "libYwAppFramework/ThirdParty/*.c", "libYwAppFramework/ThirdParty/*.cpp" },
        ["ThirdParty/libpng"] = {"libYwAppFramework/ThirdParty/libpng/*.h", "libYwAppFramework/ThirdParty/libpng/*.inl", "libYwAppFramework/ThirdParty/libpng/*.c", "libYwAppFramework/ThirdParty/libpng/*.cpp"},
        ["ThirdParty/libtarga"] = {"libYwAppFramework/ThirdParty/libtarga/*.h", "libYwAppFramework/ThirdParty/libtarga/*.inl", "libYwAppFramework/ThirdParty/libtarga/*.c", "libYwAppFramework/ThirdParty/libtarga/*.cpp"},
        ["ThirdParty/zlib"] = {"libYwAppFramework/ThirdParty/zlib/*.h", "libYwAppFramework/ThirdParty/zlib/*.inl", "libYwAppFramework/ThirdParty/zlib/*.c", "libYwAppFramework/ThirdParty/zlib/*.cpp"},
        ["Utility"] = {"libYwAppFramework/Utility/*.h", "libYwAppFramework/Utility/*.inl", "libYwAppFramework/Utility/*.c", "libYwAppFramework/Utility/*.cpp"}
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
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

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    -- filter { "system:windows" }
    --     postbuildcommands
    --     {
    --         '{MKDIR} "%{wks.location}../../Build/Bin/%{cfg.platform}/%{prj.name}"',
    --         '{COPY} "%{cfg.targetdir}/%{cfg.buildtarget.name}" "%{wks.location}../../' .. builddir .. '/Bin/%{cfg.platform}/%{prj.name}"'
    --     }

project "Demo2Model"
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
    }

    files
    { 
        "Demo2Model/YwDemoModel.h",
        "Demo2Model/YwDemoModel.cpp",
        "Demo2Model/YwDemoModelApp.h",
        "Demo2Model/YwDemoModelApp.cpp",
        "Demo2Model/YwDemoModelCamera.h",
        "Demo2Model/YwDemoModelCamera.cpp",
        "Demo2Model/YwDemoModelMain.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo2Model/Yw*.h", "Demo2Model/Yw*.inl", "Demo2Model/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    filter { "system:windows" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Wood.png"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:linux" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Wood.png"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:macosx" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Wood.png"' .. ' "' .. absdstdatadir .. '"'
        }

project "Demo3BlinnPhong"
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
    }

    files
    { 
        "Demo3BlinnPhong/YwDemoBlinnPhong.h",
        "Demo3BlinnPhong/YwDemoBlinnPhong.cpp",
        "Demo3BlinnPhong/YwDemoBlinnPhongApp.h",
        "Demo3BlinnPhong/YwDemoBlinnPhongApp.cpp",
        "Demo3BlinnPhong/YwDemoBlinnPhongCamera.h",
        "Demo3BlinnPhong/YwDemoBlinnPhongCamera.cpp",
        "Demo3BlinnPhong/YwDemoBlinnPhongMain.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo3BlinnPhong/Yw*.h", "Demo3BlinnPhong/Yw*.inl", "Demo3BlinnPhong/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    filter { "system:windows" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:linux" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:macosx" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

project "Demo4NormalMapping"
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
    }

    files
    { 
        "Demo4NormalMapping/YwDemoNormalMapping.h",
        "Demo4NormalMapping/YwDemoNormalMapping.cpp",
        "Demo4NormalMapping/YwDemoNormalMappingApp.h",
        "Demo4NormalMapping/YwDemoNormalMappingApp.cpp",
        "Demo4NormalMapping/YwDemoNormalMappingCamera.h",
        "Demo4NormalMapping/YwDemoNormalMappingCamera.cpp",
        "Demo4NormalMapping/YwDemoNormalMappingMain.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo3BlinnPhong/Yw*.h", "Demo3BlinnPhong/Yw*.inl", "Demo3BlinnPhong/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    filter { "system:windows" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Cylinder.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_color.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_nmap.bmp"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:linux" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Cylinder.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_color.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_nmap.bmp"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:macosx" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Cylinder.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_color.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/bricks_nmap.bmp"' .. ' "' .. absdstdatadir .. '"'
        }

project "Demo5TriangleShaderWireframe"
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
    }

    files
    { 
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframe.h",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframe.cpp",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeApp.h",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeApp.cpp",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeCamera.h",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeCamera.cpp",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeMain.cpp",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeShader.h",
        "Demo5TriangleShaderWireframe/YwDemoTriangleShaderWireframeShader.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo5TriangleShaderWireframe/Yw*.h", "Demo5TriangleShaderWireframe/Yw*.inl", "Demo5TriangleShaderWireframe/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    filter { "system:windows" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:linux" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:macosx" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/teapot.obj"' .. ' "' .. absdstdatadir .. '"'
        }

project "Demo6PBR"
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
        "libYwAppFramework/Resource",
        "libYwAppFramework/ThirdParty",
        "libYwAppFramework/ThirdParty/libpng",
        "libYwAppFramework/ThirdParty/libtarga",
        "libYwAppFramework/ThirdParty/zlib",
        "libYwAppFramework/Utility"
    }

    files
    { 
        "Demo6PBR/YwDemoPBR.h",
        "Demo6PBR/YwDemoPBR.cpp",
        "Demo6PBR/YwDemoPBRApp.h",
        "Demo6PBR/YwDemoPBRApp.cpp",
        "Demo6PBR/YwDemoPBRCamera.h",
        "Demo6PBR/YwDemoPBRCamera.cpp",
        "Demo6PBR/YwDemoPBRMain.cpp",
        "Demo6PBR/YwDemoPBRShader.h",
        "Demo6PBR/YwDemoPBRShader.cpp"
    }

    vpaths 
    {
        ["*"] = { "Demo6PBR/Yw*.h", "Demo6PBR/Yw*.inl", "Demo6PBR/Yw*.cpp" }
    }

    links
    {
        "libYw3d",
        "libYwAppFramework"
    }

    targetdir (appbuilddir)
    debugdir (appbuilddir)

    filter { "configurations:Debug*", "architecture:x86" }
        targetsuffix "x86D"

    filter { "configurations:Release*", "architecture:x86" }
        targetsuffix "x86"

    filter { "configurations:Debug*", "architecture:x86_64" }
        targetsuffix "D"

    filter { "system:windows" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_D.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_N.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lu_Head.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_diffuse.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_normal.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_specular.tga"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:linux" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_D.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_N.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lu_Head.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_diffuse.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_normal.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_specular.tga"' .. ' "' .. absdstdatadir .. '"'
        }

    filter { "system:macosx" }
        postbuildcommands
        {
            '{MKDIR} "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_D.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/SM_Chair_N.bmp"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lu_Head.obj"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_diffuse.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_normal.tga"' .. ' "' .. absdstdatadir .. '"',
            '{COPY} "' .. abssrcdatadir .. '/Lieutenant_head_specular.tga"' .. ' "' .. absdstdatadir .. '"'
        }
