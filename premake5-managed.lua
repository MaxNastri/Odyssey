workspace "Odyssey-Managed"
    configurations { "Debug", "Release" }
    
    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
	objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"

    group "Core"
    include "Managed/Runtime"
    group ""
