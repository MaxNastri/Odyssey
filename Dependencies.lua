include "./Scripts/premake5/Customization/ordered_pairs.lua"

-- Utility function for converting the first character to uppercase
function firstToUpper(str)
	return (str:gsub("^%l", string.upper))
end

-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")

--[[
	If you're adding a new dependency all you have to do to get it linking
	and included is define it properly in the table below, here's some example usage:

	MyDepName = {
		LibName = "my_dep_name",
		LibDir = "some_path_to_dependency_lib_dir",
		IncludeDir = "my_include_dir",
		Windows = { DebugLibName = "my_dep_name_debug" },
		Configurations = "Debug,Release"
	}

	MyDepName - This is just for organizational purposes, it doesn't actually matter for the build process
	LibName - This is the name of the .lib file that you want e.g Hazelnut to link against (you shouldn't include the .lib extension since Linux uses .a)
	LibDir - Indicates which directory the lib file is located in, this can include "%{cfg.buildcfg}" if you have a dedicated Debug / Release directory
	IncludeDir - Pretty self explanatory, the filepath that will be included in externalincludedirs
	Windows - This defines a platform-specific scope for this dependency, anything defined in that scope will only apply for Windows, you can also add one for Linux
	DebugLibName - Use this if the .lib file has a different name in Debug builds (e.g "shaderc_sharedd" vs "shaderc_shared")
	Configurations - Defines a list of configurations that this dependency should be used in, if no Configurations is specified all configs will link and include this dependency (which is what we want in most cases)

	Most of the properties I've listed can be used in either the "global" dependency scope OR in a specific platform scope,
	the only property that doesn't support that is "Configurations" which HAS to be defined in the global scope.

	Of course you can put only SOME properties in a platform scope, and others in the global scope if you want to.

	Naturally I suggest taking a look at existing dependency definitions when adding a new dependency.

	Remember that in most cases you only need to update this list, no need to manually add dependencies to the "links" list or "includedirs" list

	HEADER-ONLY LIBRARIES: If your dependency is header-only you shouldn't specify e.g LibName, just add IncludeDir and it'll be treated like a header-only library

]]--

Dependencies = {
	Coral = {
		LibName = "Coral.Native",
		LibDir = "%{wks.location}/Vendor/Coral/Build/%{cfg.buildcfg}/",
		IncludeDir = "%{wks.location}/Vendor/Coral/Coral.Native/Include/Coral",
	},
	GLFW = {
		LibName = "glfw3",
		LibDir = "%{wks.location}/Vendor/GLFW/bin/%{cfg.buildcfg}/",
		IncludeDir = "%{wks.location}/Vendor/GLFW",
	},
	EFSW = {
		LibName = "efsw-static",
		DebugLibName = "efsw-static-debug",
		LibDir = "%{wks.location}/Vendor/efsw/Lib/%{cfg.buildcfg}/",
		IncludeDir = "%{wks.location}/Vendor/efsw/include/efsw",
	},
	Entt = {
		IncludeDir = "%{wks.location}/Vendor/entt/include",
	},
	MagicEnum = {
		IncludeDir = "%{wks.location}/Vendor/magic_enum/Include",
	},
	TinyGLTF = {
		IncludeDir = "%{wks.location}/Vendor/tinygltf/Include",
	},
	ShaderC = {
		LibName = "shaderc_combined",
	},
	SPIRVCrossCore = {
		LibName = "spirv-cross-core",
	},
	SPIRVCrossGLSL = {
		LibName = "spirv-cross-glsl",
	},
	SPIRVCrossHLSL = {
		LibName = "spirv-cross-hlsl",
	},
	SPIRVCrossReflect = {
		LibName = "spirv-cross-reflect",
	},
	SPIRVCrossUtil = {
		LibName = "spirv-cross-util",
	},
	Vulkan = {
		LibName = "glfw3",
		LibDir = "%{wks.location}/Vendor/Vulkan/Lib/%{cfg.buildcfg}/",
		IncludeDir = "%{wks.location}/Vendor/Vulkan/Include",
	},
}

-- NOTE(Peter): Probably don't touch these functions unless you know what you're doing (or just ask me if you need help extending them)

function LinkDependency(table, is_debug, target)

	-- Setup library directory
	if table.LibDir ~= nil then
		libdirs { table.LibDir }
	end

	-- Try linking
	local libraryName = nil
	if table.LibName ~= nil then
		libraryName = table.LibName
	end

	if table.DebugLibName ~= nil and is_debug and target == "Windows" then
		libraryName = table.DebugLibName
	end

	if libraryName ~= nil then
		links { libraryName }
		return true
	end

	return false
end

function AddDependencyIncludes(table)
	if table.IncludeDir ~= nil then
		externalincludedirs { table.IncludeDir }
	end
end

function ProcessDependencies(config_name)
	local target = firstToUpper(os.target())

	for key, libraryData in orderedPairs(Dependencies) do

		-- Always match config_name if no Configurations list is specified
		local matchesConfiguration = true

		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end

		local isDebug = config_name == "Debug"

		if matchesConfiguration then
			local continueLink = true

			-- Process Platform Scope
			if libraryData[target] ~= nil then
				continueLink = not LinkDependency(libraryData[target], isDebug, target)
				AddDependencyIncludes(libraryData[target])
			end

			-- Process Global Scope
			if continueLink then
				LinkDependency(libraryData, isDebug, target)
			end

			AddDependencyIncludes(libraryData)
		end

	end
end

function IncludeDependencies(config_name)
	local target = firstToUpper(os.target())

	for key, libraryData in orderedPairs(Dependencies) do

		-- Always match config_name if no Configurations list is specified
		local matchesConfiguration = true

		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end

		if matchesConfiguration then
			-- Process Global Scope
			AddDependencyIncludes(libraryData)

			-- Process Platform Scope
			if libraryData[target] ~= nil then
				AddDependencyIncludes(libraryData[target])
			end
		end

	end
end
