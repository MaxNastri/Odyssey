local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Engine"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    architecture "x86_64"
    staticruntime "Off"
    
    flags { "NoPCH", "MultiProcessorCompile" }
    
    pchheader "PCH.hpp"
    pchsource "Source/PCH.cpp"

    forceincludes { "PCH.hpp" }

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
        "Source/**.c",
    }

    includedirs {
        "Include",
        "Include/**",
        "Source",
        "Source/**",
    }

    IncludeDependencies()
    
    defines {
        "CORAL_WINDOWS",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_FORCE_LEFT_HANDED",
        "IMGUI_DEFINE_MATH_OPERATORS",
        "SPDLOG_USE_STD_FORMAT",
        "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
        "VK_NO_PROTOTYPES",
        "YAML_CPP_STATIC_DEFINE",
    }

    filter "action:vs*"
        linkoptions { "/ignore:4099", "/ignore:4006" } -- Disable no PDB found warning
        disablewarnings { "4068" } -- Disable "Unknown #pragma mark warning"
        
    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"