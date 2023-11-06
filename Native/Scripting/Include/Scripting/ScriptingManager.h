#pragma once
#include <filesystem>
#include <functional>

#include "HostInstance.hpp"
#include "ManagedObject.hpp"
#include "Assembly.hpp"
#include "GC.hpp"
#include "NativeArray.hpp"
#include "Attribute.hpp"
#include <Stopwatch.h>

namespace Odyssey::Scripting
{
	class ScriptingManager
	{
	public:
		static void Initialize(std::filesystem::path exeDir);
		static Coral::ManagedAssembly& LoadAssembly(std::string_view path);
		static void Recompile();

		static Coral::ManagedObject CreateManagedObject(const std::string& fqManagedClassName);

	private:
		static Coral::HostInstance hostInstance;
		static Coral::AssemblyLoadContext loadContext;
		static Coral::ManagedAssembly userAssembly;

		static Odyssey::Framework::Stopwatch stopwatch;
		static bool running;

		const float MaxFPS = 1.0f / 144.0f;
	};
}