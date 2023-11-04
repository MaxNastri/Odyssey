local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

include "DebuggerTypeExtension.lua"

project "Odyssey.Native.Editor"
    language "C++"
    cppdialect "C++20"
    kind "ConsoleApp"
    staticruntime "Off"
    debuggertype "NativeWithManagedCore"
    
    architecture "x86_64"
    
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
        "%{wks.location}/Native/Framework/Include",
        "%{wks.location}/Native/Framework/Include/**",
        "%{wks.location}/Native/Scripting/Include",
        "%{wks.location}/Native/Scripting/Include/**",
    }
    
    includedirs {
        "Include",
        "Include/**",
    }

    libdirs {
        "%{cfg.targetdir}",
    }

    links {
        "Odyssey.Native.Framework.lib",
        "Odyssey.Native.Scripting.lib",
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