#pragma once
#include <filesystem>
#include <Windows.h>

namespace Odyssey::Scripting::Paths
{
	namespace Absolute
	{
		std::filesystem::path GetApplicationPath()
		{
			wchar_t buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, sizeof(buffer));
			std::filesystem::path outPath = std::filesystem::path(buffer).parent_path();
			return outPath;
		}
	}
	namespace Relative
	{
		const char* ExampleManagedProject = "..\\..\\Managed\\Example\\Odyssey.Managed.Example.csproj";
	}
}