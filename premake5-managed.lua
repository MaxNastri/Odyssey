workspace "Odyssey-Managed"
    configurations { "Debug", "Release" }
    
    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
	objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"

include "Managed/Core"
include "Managed/Example"
