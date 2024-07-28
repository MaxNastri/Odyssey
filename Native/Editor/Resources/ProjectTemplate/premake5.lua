FileVersion = 1.0

workspace "%ProjectName%"
    startproject "%ProjectName%"
    configurations { "Debug", "Release"}

group "Odyssey"

group ""

project "%ProjectName%"
    location "Assets/Scripts"
    kind "SharedLib"
    language "C#"
    dotnetframework "net8.0"

    targetname "%ProjectName%"
    targetdir ("%{wks.location}/Cache/Build/Binaries")
    objdir ("%{wks.location}/Cache/Build/Intermediates")

    files  {
        "Assets/Scripts/**.cs", 
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"