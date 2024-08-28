local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Odyssey.Engine"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    staticruntime "Off"
    
    architecture "x86_64"
    
    flags { "NoPCH", "MultiProcessorCompile" }
    
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
        "Source/**.c",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/**.h",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/**.hpp",
        "%{wks.location}/Vendor/Coral/Coral.Native/Source/Coral/**.cpp",
        "%{wks.location}/Vendor/Coral/NetCore/**.h",
    }

    includedirs {
        "Include",
        "Include/**",
        "Source",
        "Source/**",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/",
        "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral/**",
        "%{wks.location}/Vendor/Coral/NetCore/**",
        "%{wks.location}/Vendor/Coral/NetCore/",
    }
    externalincludedirs {
        "%{wks.location}/Vendor/glfw3/",
        "%{wks.location}/Vendor/Vulkan/Include/",
        "%{wks.location}/Vendor/assimp/include/",
        "%{wks.location}/Vendor/efsw/include/efsw",
        "%{wks.location}/Vendor/entt/include/",
    }

    libdirs {
        "%{cfg.targetdir}",
        "%{wks.location}/Vendor/Vulkan/Lib/",
        "%{wks.location}/Vendor/efsw/lib/",
    }

    links {
        "glfw3.lib",
        "assimp-vc143-mtd.lib",
        "shaderc_combined.lib",
        "spirv-cross-core.lib",
        "spirv-cross-glsl.lib",
        "spirv-cross-hlsl.lib",
        "spirv-cross-reflect.lib",
        "spirv-cross-util.lib",
        "efsw-static-debug.lib",
    }

    
	filter "files:Source/**.c"

    filter { "system:windows" }
        prebuildcommands {
			'{COPYFILE} "%{wks.location}/Vendor/glfw3/lib/glfw3.lib" "%{cfg.targetdir}"',
            '{COPYFILE} "%{wks.location}/Vendor/assimp/lib/assimp-vc143-mtd.dll" "%{cfg.targetdir}"',
			'{COPYFILE} "%{wks.location}/Vendor/assimp/lib/assimp-vc143-mtd.lib" "%{cfg.targetdir}"',
        }
	filter {}

    defines {
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_FORCE_LEFT_HANDED",
        "YAML_CPP_STATIC_DEFINE",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "VK_NO_PROTOTYPES",
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    filter "action:vs*"
        linkoptions { "/ignore:4099" } -- NOTE(Peter): Disable no PDB found warning
        disablewarnings { "4068" } -- Disable "Unknown #pragma mark warning"
        
    filter { "system:windows" }
		defines { "CORAL_WINDOWS" }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"