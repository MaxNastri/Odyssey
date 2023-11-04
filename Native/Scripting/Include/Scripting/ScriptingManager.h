#pragma once
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
		void Initialize(std::filesystem::path exeDir);
		Coral::ManagedAssembly& LoadAssembly(std::string_view path);
		void Run();
		void Stop();

	private:
		Coral::HostInstance hostInstance;
		Coral::AssemblyLoadContext loadContext;
		std::vector<Coral::ManagedAssembly> loadedAssemblies;

		Odyssey::Framework::Stopwatch stopwatch;
		bool running;

		const float MaxFPS = 1.0f / 144.0f;

		// Testing
		Coral::ManagedObject exampleInstance;
	};
}