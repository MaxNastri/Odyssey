#include "ScriptingManager.h"
#include "ScriptCompiler.h"
#include <Log.h>

namespace Odyssey::Scripting
{
	Coral::HostInstance ScriptingManager::hostInstance;
	Coral::AssemblyLoadContext ScriptingManager::loadContext;
	Coral::ManagedAssembly ScriptingManager::userAssembly;

	Odyssey::Framework::Stopwatch ScriptingManager::stopwatch;
	bool ScriptingManager::running;

	void ExceptionCallback(std::string_view exception)
	{
		Framework::Log::Error(exception.data());
	}

	void ScriptingManager::Initialize(std::filesystem::path exeDir)
	{
		Framework::Log::Info("Initializing scripting...");

		std::string coralDir = exeDir.string();
		Coral::HostSettings settings =
		{
			.CoralDirectory = coralDir,
			.ExceptionCallback = ExceptionCallback
		};

		hostInstance.Initialize(settings);
		loadContext = hostInstance.CreateAssemblyLoadContext("ExampleContext");

		// Load the assembly
		std::filesystem::path assemblyPath = exeDir / "Odyssey.Managed.Example.dll";
		userAssembly = LoadAssembly(assemblyPath.string());
	}

	Coral::ManagedAssembly& ScriptingManager::LoadAssembly(std::string_view path)
	{
		return loadContext.LoadAssembly(path);
	}

	void ScriptingManager::Recompile()
	{
		// Get the assemblies of this context
		Coral::StableVector<Coral::ManagedAssembly> assemblies = loadContext.GetLoadedAssemblies();

		// Cache the paths of all assemblies
		std::vector<std::string_view> assemblyPaths;
		for (int i = 0; i < assemblies.GetElementCount(); ++i)
		{
			assemblyPaths.push_back(assemblies[i].GetPath());
		}

		// Reload the assembly context
		hostInstance.UnloadAssemblyLoadContext(loadContext);

		// Recompile and build
		ScriptCompiler::CompileAssembly();

		loadContext = hostInstance.CreateAssemblyLoadContext("ExampleContext");

		Coral::ManagedAssembly assembly;
		// Load the assemblies inside the context
		for (int i = 0; i < assemblyPaths.size(); ++i)
		{
			assembly = loadContext.LoadAssembly(assemblyPaths[i]);
		}
	}

	Coral::ManagedObject ScriptingManager::CreateManagedObject(const std::string& fqManagedClassName)
	{
		// TODO: insert return statement here
		Coral::Type& managedType = userAssembly.GetType(fqManagedClassName);
		return managedType.CreateInstance();
	}
}