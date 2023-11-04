include "../CSExtensions.lua"

project "Odyssey.Managed.Example"
    language "C#"
    dotnetframework "net7.0"
    kind "SharedLib"
	clr "Unsafe"
	
    propertytags {
        { "AppendTargetFrameworkToOutputPath", "false" },
        { "Nullable", "enable" }
    }

    files {
        "Source/**.cs"
    }
    
    links { "Odyssey.Managed.Core" }
