include "CSExtensions.lua"

project "Odyssey.Framework"
    language "C#"
    dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"
    targetname "Odyssey.Framework"
    namespace "Odyssey"

    targetdir "%{wks.location}/Build/%{cfg.buildcfg}"
    objdir "%{wks.location}/Intermediates/%{cfg.buildcfg}"
    
	links {
		"%{cfg.targetdir}/Coral.Managed"
	}

    -- Don't specify architecture here. (see https://github.com/premake/premake-core/issues/1758)
    propertytags {
        { "AppendTargetFrameworkToOutputPath", "false" },
    }

    disablewarnings {
        "CS8500"
    }

    files {
        "%{wks.location}/Projects/Framework/Source/**.cs",
    }
        
    filter { "system:windows" }
        postbuildcommands {
            '{COPYFILE} "%{cfg.targetdir}/Odyssey.Framework.dll", "%{wks.location}/Projects/Editor/Resources/Scripts"',
            '{COPYFILE} "%{cfg.targetdir}/Odyssey.Framework.pdb", "%{wks.location}/Projects/Editor/Resources/Scripts"',
        }