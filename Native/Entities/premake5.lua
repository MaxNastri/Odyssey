local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Native.Entities"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    staticruntime "Off"
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

    includedirs {
        "Include",
        "Include/**",
        "Source",
        "Source/**",
    }

    externalincludedirs {
        "%{wks.location}/Native/Framework/Include/",
        "%{wks.location}/Native/Framework/Include/**",
    }
    
    libdirs {
        "%{cfg.targetdir}",
    }

    links {
        "Odyssey.Native.Framework.lib",
    }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"