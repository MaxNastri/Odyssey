premake.api.addAllowed("debuggertype", "NativeWithManagedCore")

workspace "Odyssey-Native"
    configurations { "Debug", "Release" }

    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
	objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"
    
    startproject "Odyssey.Native.Editor"
    
	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

include "Native/Editor"
include "Native/Scripting"