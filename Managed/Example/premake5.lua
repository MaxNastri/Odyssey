project "Odyssey.Managed.Example"
    language "C#"
    dotnetframework "net8.0"
    kind "SharedLib"
	clr "Unsafe"
	
    files {
        "Source/**.cs"
    }
    
    links {
        "Coral.Managed",
    }
