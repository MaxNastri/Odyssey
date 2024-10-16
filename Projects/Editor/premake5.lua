local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

include "DebuggerTypeExtension.lua"

project "Odyssey.Editor"
    language "C++"
    cppdialect "C++20"
    kind "ConsoleApp"
    architecture "x86_64"
    staticruntime "Off"
    debuggertype "NativeWithManagedCore"
    dependson "Odyssey.Engine"

    flags { "MultiProcessorCompile" }
    
    pchheader "PCH.hpp"
    pchsource "Source/PCH.cpp"

    forceincludes { "PCH.hpp" }
    
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
    }
    
    links {
        "Odyssey.Engine",
    }
    
    defines {
        "ODYSSEY_EDITOR"
    }

    filter { "system:windows" }
        postbuildcommands {
            '{COPYFILE} "%{wks.location}/Vendor/Coral/Build/Debug/Coral.Managed.dll", "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Vendor/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json", "%{cfg.targetdir}"',
        }

    filter "action:vs*"
        linkoptions { "/ignore:4098", "/ignore:4099" } -- Disable no PDB found warning
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