local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

include "DebuggerTypeExtension.lua"

project "Odyssey.Editor"
    language "C++"
    cppdialect "C++20"
    kind "ConsoleApp"
    staticruntime "Off"
    debuggertype "NativeWithManagedCore"
    dependson "Odyssey.Engine"

    architecture "x86_64"
    
    flags { "MultiProcessorCompile" }
    
    pchheader "PCH.hpp"
    pchsource "Source/PCH.cpp"

    forceincludes { "PCH.hpp" }

    filter { "action:xcode4" }
        pchheader "Source/PCH.hpp"
    filter { }

    files {
        "Source/**.h",
        "Source/**.inl",
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.hlsl",
    }

    includedirs {
        "Source",
        "Source/**",
    }

    externalincludedirs {
        "%{wks.location}/Projects/Engine/Include",
        "%{wks.location}/Projects/Engine/Include/**",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/**",
        --"%{wks.location}/Vendor/Vulkan/Include/",
        "%{wks.location}/Vendor/efsw/include/efsw",
        "%{wks.location}/Vendor/entt/include/",
        "%{wks.location}/Vendor/tinygltf/Include/",
    }

    libdirs {
        "%{cfg.targetdir}",
        --"%{wks.location}/Vendor/Vulkan/Lib/",
        "%{wks.location}/Vendor/efsw/lib/",
    }

    links {
        "Odyssey.Engine.lib",
        --"shaderc_combined.lib",
        --"spirv-cross-core.lib",
        --"spirv-cross-glsl.lib",
        --"spirv-cross-hlsl.lib",
        --"spirv-cross-reflect.lib",
        --"spirv-cross-util.lib",
        "efsw-static-debug.lib",
    }
    
    defines {
        "ODYSSEY_EDITOR",
        "VK_NO_PROTOTYPES"
    }

    filter { "system:windows" }
        postbuildcommands {
            '{COPYFILE} "%{wks.location}/Vendor/Coral/Build/Debug/Coral.Managed.dll", "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Vendor/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json", "%{cfg.targetdir}"',
        }

    filter "action:vs*"
        linkoptions { "/ignore:4098", "/ignore:4099" } -- NOTE(Peter): Disable no PDB found warning
        disablewarnings { "4068" } -- Disable "Unknown #pragma mark warning"
        
    filter { "configurations:Debug" }
        runtime "Debug"
		symbols "On"
        defines { "ODYSSEY_DEBUG" }
        ProcessDependencies("Debug")
        
    filter { "configurations:Release" }
        runtime "Release"
        defines { "ODYSSEY_RELEASE" }
        ProcessDependencies("Release")