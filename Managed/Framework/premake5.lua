include "CSExtensions.lua"

project "Odyssey.Managed.Framework"
    language "C#"
    dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"

    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
    objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"
    
	links {
		"Coral.Managed"
	}

    -- Don't specify architecture here. (see https://github.com/premake/premake-core/issues/1758)
    propertytags {
        { "AppendTargetFrameworkToOutputPath", "false" },
        { "Nullable", "enable" },
    }

    disablewarnings {
        "CS8500"
    }

    files {
        "%{wks.location}/Managed/Framework/Source/**.cs",
    }
        