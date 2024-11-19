project "JoltPhysics"
    language "C++"
    cppdialect "C++17"
    kind "StaticLib"
    architecture "x86_64"
    staticruntime "off"

    flags { "MultiProcessorCompile" }

    targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files
    {
        "JoltPhysics/Jolt/**.cpp",
        "JoltPhysics/Jolt/**.h",
        "JoltPhysics/Jolt/**.inl",
        "JoltPhysics/Jolt/**.gliffy"
    }

    includedirs { "JoltPhysics/Jolt", "JoltPhysics/" }

    filter "system:windows"
        systemversion "latest"

        files { "JoltPhysics/Jolt/Jolt.natvis" }

    filter "not system:windows"
        forceincludes { "stdint.h", "stdarg.h" }

    filter "configurations:Debug or configurations:Debug-AS"
        symbols "on"
        optimize "off"

        defines
        {
            "_DEBUG",
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_ENABLE_ASSERTS",
            "JPH_PROFILE_ENABLED",
        }

    filter { "system:windows", "configurations:Debug-AS" }
		sanitize { "Address" }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

    filter "configurations:Release"
        optimize "speed"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }

        defines
        {
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
            "JPH_BUILD_SHARED_LIBRARY",
        }

    filter "configurations:Dist"
        optimize "speed"
        symbols "off"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }