include "CSExtensions.lua"
OdysseyRootDirectory = os.getenv("ODYSSEY_ROOT")
ProjectDirectory = path.join(OdysseyRootDirectory, "Projects/Framework")
TargetDirectory = path.join(OdysseyRootDirectory, "Projects/Editor/Resources/Scripts")

project "Odyssey.Framework"
    language "C#"
    dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"
    namespace "Odyssey"

    targetdir "%{TargetDirectory}"
    objdir "%{TargetDirectory}/Intermediates"
    
	links {
		"%{TargetDirectory}/Coral.Managed"
	}

    -- Don't specify architecture here. (see https://github.com/premake/premake-core/issues/1758)
    propertytags {
        { "AppendTargetFrameworkToOutputPath", "false" },
    }

    disablewarnings {
        "CS8500"
    }

    files {
        "%{ProjectDirectory}/Source/**.cs",
    }