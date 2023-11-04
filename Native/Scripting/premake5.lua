local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Native.Scripting"
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

    externalincludedirs {
        "%{wks.location}/Vendor/NetCore/7.0.7/",
        "%{wks.location}/Native/Framework/Include/",
        "%{wks.location}/Native/Framework/Include/**",
    }
    
    filter { "system:windows" }

		postbuildcommands {
			'{ECHO} Copying "%{wks.location}/Vendor/NetCore/7.0.7/nethost.dll" to "%{cfg.targetdir}"',
			'{COPYFILE} "%{wks.location}/Vendor/NetCore/7.0.7/nethost.dll" "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Managed/Core/Odyssey.Managed.Core.runtimeconfig.json" "%{cfg.targetdir}"',
		}
	filter {}

    libdirs {
        "%{cfg.targetdir}",
        "%{wks.location}/Vendor/NetCore/7.0.7/"
    }

    links {
        "nethost.lib",
        "Odyssey.Native.Framework.lib",
    }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"