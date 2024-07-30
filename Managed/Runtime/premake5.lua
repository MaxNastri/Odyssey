
project "Coral.Managed"
    language "C#"
    dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"

    -- Don't specify architecture here. (see https://github.com/premake/premake-core/issues/1758)
    
    disablewarnings {
        "CS8500"
    }

    files {
        "Source/**.cs",
        "%{wks.location}/Vendor/Coral/Coral.Managed/Source/**.cs",
    }