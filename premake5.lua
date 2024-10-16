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

group "Odyssey"
include "Projects/Editor"
include "Projects/Engine"
include "Projects/Framework"
group ""