#pragma once
#ifndef PATHS_H
#define PATHS_H
#include <filesystem>
#include <Windows.h>

namespace Odyssey::Scripting::Paths
{
	class Absolute
	{
	public:
		static std::filesystem::path GetApplicationPath()
		{
			wchar_t buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, sizeof(buffer));
			std::filesystem::path outPath = std::filesystem::path(buffer).parent_path();
			return outPath;
		}
	};

	class Relative
	{
	public:
		inline static const char* ExampleManagedProject = "..\\..\\Managed\\Example\\Odyssey.Managed.Example.csproj";
	};
}
#endif