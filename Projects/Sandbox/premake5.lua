FileVersion = 1.0
OdysseyRootDirectory = os.getenv("ODYSSEY_ROOT")
ProjectDirectory = path.join(OdysseyRootDirectory, "Projects/Sandbox")
TargetDirectory = path.join(ProjectDirectory, "Cache/Build/Binaries")

workspace "Sandbox"
    startproject "Sandbox"
    configurations { "Debug", "Release"}
    
project "Sandbox"
    location "Assets/Scripts"
    kind "SharedLib"
    language "C#"
    dotnetframework "net8.0"
    targetname "Sandbox"
    namespace "Sandbox"
    
    targetdir (TargetDirectory)
    objdir (path.join(ProjectDirectory, "Cache/Build/Intermediates"))

    files  {
        "Assets/Scripts/**.cs", 
    }

    links {
        path.join(ProjectDirectory, "Cache/UserAssemblies/Coral.Managed"),
        path.join(ProjectDirectory, "Cache/UserAssemblies/Odyssey.Framework"),
    }
    
    filter { "system:windows" }
        postbuildcommands {
            '{COPYFILE} ' .. path.join(TargetDirectory, "net8.0/Sandbox.dll") .. ', ' .. path.join(ProjectDirectory, "Cache/UserAssemblies"),
            '{COPYFILE} ' .. path.join(TargetDirectory, "net8.0/Sandbox.pdb") .. ', ' .. path.join(ProjectDirectory, "Cache/UserAssemblies"),
        }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"