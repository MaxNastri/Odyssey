include "../CSExtensions.lua"

project "Odyssey.Managed.Core"
    language "C#"
    dotnetframework "net7.0"
    kind "SharedLib"
	clr "Unsafe"
    
	targetdir("%{wks.location}/Build/%{cfg.buildcfg}-%{cfg.system}")
	objdir("%{wks.location}/Intermediates/%{cfg.buildcfg}-%{cfg.system}")

    propertytags {
        { "AppendTargetFrameworkToOutputPath", "false" },
        { "Nullable", "enable" },
    }

    disablewarnings {
        "CS8500"
    }

    files {
        "Source/**.cs"
    }
