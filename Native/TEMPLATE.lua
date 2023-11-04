project "Odyssey.Native.Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir)
    objdir ("%{wks.location}/build/" .. outputdir)

    files
    {
        "include/**.h",
        "include/**.inl",
        "include/**.cpp",
        "include/**.hpp",
        "include/**.hlsl",
        "source/**.h",
        "source/**.inl",
        "source/**.cpp",
        "source/**.hpp",
        "source/**.hlsl",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

    includedirs
    {
        "include",
        "include/**",
    }

    libdirs
    {
    }

    links
    {
    }

    linkoptions
    {
        '/ignore:4006'
    }

    filter { "files:**.hlsl"}
        flags "ExcludeFromBuild"
        shadermodel "5.0"

    filter { "files:**.pixel.hlsl"}
        removeflags "ExcludeFromBuild"
        shadertype "Pixel"

    filter { "files:**.vertex.hlsl"}
        removeflags "ExcludeFromBuild"
        shadertype "Vertex"

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "WIN32_LEAN_AND_MEAN"
        }

    filter "configurations:Debug"
        defines "ODYSSEY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Debug"
        defines "ODYSSEY_RELEASE"
        runtime "Debug"
        symbols "on"