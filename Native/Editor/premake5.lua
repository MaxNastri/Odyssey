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
        "%{wks.location}/Native/Scripting/Include",
        "%{wks.location}/Native/Scripting/Include/**",
        "%{wks.location}/Vendor/NetCore/7.0.7/"
    }
    
    filter { "system:windows" }
	    libdirs { "%{wks.location}/Vendor/NetCore/7.0.7/" }

		postbuildcommands {
			'{ECHO} Copying "%{wks.location}/Vendor/NetCore/7.0.7/nethost.dll" to "%{cfg.targetdir}"',
			'{COPYFILE} "%{wks.location}/Vendor/NetCore/7.0.7/nethost.dll" "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Managed/Core/Odyssey.Managed.Core.runtimeconfig.json" "%{cfg.targetdir}"',
		}
	filter {}

    includedirs {
        "Include",
        "Include/**",
    }

    libdirs {
        "%{cfg.targetdir}",
        "%{wks.location}/Vendor/NetCore/7.0.7/"
    }

    links {
        "Odyssey.Native.Scripting.lib",
        "nethost.lib"
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