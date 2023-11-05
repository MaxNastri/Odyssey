#include "ScriptingManager.h"
#include "ScriptCompiler.h"
#include <Log.h>

namespace Odyssey::Scripting
{
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
		Coral::ManagedAssembly& assembly = LoadAssembly(assemblyPath.string());

		// Get the type
		Coral::Type& exampleType = assembly.GetType("Example.Managed.ExampleScript");

		// Create an instance of type Example.Managed.ExampleClass and pass 50 to the constructor
		exampleInstance = exampleType.CreateInstance();
	}

	Coral::ManagedAssembly& ScriptingManager::LoadAssembly(std::string_view path)
	{
		return loadContext.LoadAssembly(path);
	}

	void ScriptingManager::UpdateScripts()
	{
		exampleInstance.InvokeMethod("Void Update()");
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

		// Destroy the object associated with the assembly
		exampleInstance.Destroy();

		// Reload the assembly context
		hostInstance.UnloadAssemblyLoadContext(loadContext);

		ScriptCompiler::CompileAssembly();

		loadContext = hostInstance.CreateAssemblyLoadContext("ExampleContext");

		Coral::ManagedAssembly assembly;
		// Load the assemblies inside the context
		for (int i = 0; i < assemblyPaths.size(); ++i)
		{
			assembly = loadContext.LoadAssembly(assemblyPaths[i]);
		}

		// Get the type
		Coral::Type& exampleType = assembly.GetType("Example.Managed.ExampleScript");

		// Remake the object
		exampleInstance = exampleType.CreateInstance();
	}
}