workspace "Odyssey"
    architecture "x64"
    startproject "projects/Odyssey.Native.Editor"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
libdir = "%{wks.location}/bin"
vendorLibDir = "%{wks.location}/vendor/bin"

LibDir = {}
LibDir["glfw3"] = "%{wks.location}/projects/vendor/glfw3/lib"
LibDir["vulkan"] = "%{wks.location}/projects/vendor/vulkan/lib"

IncludeDir = {}
-- Custom Projects
IncludeDir["Engine"] = "%{wks.location}/projects/Odyssey.Engine/include"
-- Vendors
IncludeDir["vendor"] = "%{wks.location}/projects/vendor"
IncludeDir["cereal"] = "%{wks.location}/projects/vendor/cereal"
IncludeDir["glfw"] = "%{wks.location}/projects/vendor/glfw3"
IncludeDir["glm"] = "%{wks.location}/projects/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/projects/vendor/stb_image"
IncludeDir["vulkan"] = "%{wks.location}/projects/vendor/vulkan"
IncludeDir["spdlog"] = "%{wks.location}/projects/vendor/spdlog/include/spdlog"

include "projects/Odyssey.Native.Editor"