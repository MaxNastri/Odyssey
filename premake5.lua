premake.api.addAllowed("debuggertype", "NativeWithManagedCore")

workspace "Odyssey"
    configurations { "Debug", "Release" }

    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
	objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"
    
    startproject "Odyssey.Native.Editor"
    
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

group "Native"
include "Native/Editor"
include "Native/Engine"
group ""
group "Managed"
include "Managed/Framework"
group ""