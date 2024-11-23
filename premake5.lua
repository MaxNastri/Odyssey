include "Dependencies.lua"
premake.api.addAllowed("debuggertype", "NativeWithManagedCore")

workspace "Odyssey"
    configurations { "Debug", "Release" }
	
    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
	objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"
    
    startproject "Odyssey.Editor"
    
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "language:C++ or language:C"
		architecture "x86_64"
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}"

group "Dependencies"
include "Vendor/Jolt/JoltPhysicsPremake.lua"
include "Vendor/efsw/premake5.lua"
include "Vendor/GLFW/premake5.lua"
group ""

group "Odyssey"
include "Projects/Editor"
include "Projects/Engine"
include "Projects/Framework"
group ""