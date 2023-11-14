local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Native.Graphics"
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
        "%{wks.location}/Vendor/glfw3/",
        "%{wks.location}/Vendor/Vulkan/Include/",
        "%{wks.location}/Native/Framework/Include/",
        "%{wks.location}/Native/Framework/Include/**",
    }
    
    libdirs {
        "%{cfg.targetdir}",
    }

    links {
        "glfw3.lib",
        "vulkan-1.lib",
        "Odyssey.Native.Framework.lib",
    }
    
    filter { "system:windows" }
        prebuildcommands {
			'{ECHO} Copying "%{wks.location}/Vendor/glfw3/lib/glfw3.lib" to "%{cfg.targetdir}"',
			'{COPYFILE} "%{wks.location}/Vendor/glfw3/lib/glfw3.lib" "%{cfg.targetdir}"',
			'{COPYFILE} "%{wks.location}/Vendor/Vulkan/Lib/vulkan-1.lib" "%{cfg.targetdir}"',
        }
	filter {}

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"