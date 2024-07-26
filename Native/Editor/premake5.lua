local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

include "DebuggerTypeExtension.lua"

project "Odyssey.Native.Editor"
    language "C++"
    cppdialect "C++20"
    kind "ConsoleApp"
    staticruntime "Off"
    debuggertype "NativeWithManagedCore"
    
    architecture "x86_64"
    
    flags { "MultiProcessorCompile" }
    
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

    externalincludedirs {
        "%{wks.location}/Native/Engine/Include",
        "%{wks.location}/Native/Engine/Include/**",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/**",
        "%{wks.location}/Vendor/Vulkan/Include/",
        "%{wks.location}/Vendor/assimp/include/",
    }
    
    includedirs {
        "Include",
        "Include/**",
    }

    libdirs {
        "%{cfg.targetdir}",
        "%{wks.location}/Vendor/Vulkan/Lib/"
    }

    links {
        "Odyssey.Native.Engine.lib",
        "assimp-vc143-mt.lib",
        "shaderc_combined.lib",
        "spirv-cross-core.lib",
        "spirv-cross-glsl.lib",
        "spirv-cross-hlsl.lib",
        "spirv-cross-reflect.lib",
        "spirv-cross-util.lib"
    }

    filter { "configurations:Debug" }
        runtime "Debug"
		symbols "On"
        defines { "ODYSSEY_DEBUG" }
	filter {}

    filter { "configurations:Release" }
        runtime "Release"
        defines { "ODYSSEY_RELEASE" }
	filter {}