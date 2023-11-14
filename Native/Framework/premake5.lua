local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Native.Framework"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    staticruntime "Off"
    
    architecture "x86_64"
    
    pchheader "PCH.hpp"
    pchsource "Source/PCH.cpp"

    forceincludes { "PCH.hpp" }

    filter { "action:xcode4" }
        pchheader "Source/PCH.hpp"
    filter { }

    files {
        "Include/**.h",
        "Include/**.inl",
        "Include/**.cpp",
        "Include/**.hpp",
        "Include/**.hlsl",
        "Source/**.h",
        "Source/**.inl",
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.hlsl",
    }

    includedirs {
        "Include",
        "Include/**",
        "Source",
        "Source/**",
    }
    defines { "GLM_FORCE_DEPTH_ZERO_TO_ONE", "YAML_CPP_STATIC_DEFINE" }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"