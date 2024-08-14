FileVersion = 1.0

workspace "ExampleProject"
    startproject "ExampleProject"
    configurations { "Debug", "Release"}

project "ExampleProject"
    location "Assets/Scripts"
    kind "SharedLib"
    language "C#"
    dotnetframework "net8.0"

    targetname "ExampleProject"
    targetdir ("%{wks.location}/Cache/Build/Binaries")
    objdir ("%{wks.location}/Cache/Build/Intermediates")

    files  {
        "Assets/Scripts/**.cs", 
    }
    
    links {
    "%{wks.location}/Cache/UserAssemblies/Coral.Managed",
    "%{wks.location}/Cache/UserAssemblies/Odyssey.Framework"
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"