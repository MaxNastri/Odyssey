#include "ScriptingManager.h"
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
}