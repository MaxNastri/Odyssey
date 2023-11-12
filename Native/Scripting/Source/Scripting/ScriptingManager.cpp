#include "ScriptingManager.h"
#include "ScriptCompiler.h"
#include "Paths.h"
#include <Logger.h>

namespace Odyssey::Scripting
{
	Coral::HostInstance ScriptingManager::hostInstance;
	Coral::HostSettings ScriptingManager::hostSettings;
	Coral::AssemblyLoadContext ScriptingManager::userAssemblyContext;
	Coral::ManagedAssembly ScriptingManager::userAssembly;
	ScriptCompiler ScriptingManager::scriptCompiler;
	std::vector<Coral::ManagedObject> ScriptingManager::managedObjects;

	void ExceptionCallback(std::string_view exception)
	{
		Framework::Logger::LogError(exception.data());
	}

	void ScriptingManager::Initialize()
	{
		// Initialize Coral
		std::filesystem::path appPath = Paths::Absolute::GetApplicationPath();
		std::string coralDir = appPath.string();
		hostSettings =
		{
			.CoralDirectory = coralDir,
			.CoralFilename = "Coral.Managed.dll",
			.ExceptionCallback = ExceptionCallback
		};
		hostInstance.Initialize(hostSettings);

		// Build and load the user assemblies
		//scriptCompiler.CompileUserAssembly();
		LoadUserAssemblies();
	}

	void ScriptingManager::LoadUserAssemblies()
	{
		userAssemblyContext = hostInstance.CreateAssemblyLoadContext("UserScripts");

		// Load the assembly
		std::filesystem::path appPath = Paths::Absolute::GetApplicationPath();
		std::filesystem::path assemblyPath = appPath / UserAssemblyFilename;
		userAssembly = userAssemblyContext.LoadAssembly(assemblyPath.string());
	}

	void ScriptingManager::UnloadUserAssemblies()
	{
		for (Coral::ManagedObject& object : managedObjects)
		{
			object.Destroy();
		}
		managedObjects.clear();

		hostInstance.UnloadAssemblyLoadContext(userAssemblyContext);
	}

	bool ScriptingManager::RecompileUserAssemblies()
	{
		return scriptCompiler.CompileUserAssembly();
	}

	void ScriptingManager::ReloadUserAssemblies()
	{
		UnloadUserAssemblies();
		LoadUserAssemblies();
	}

	Coral::ManagedObject ScriptingManager::CreateManagedObject(const std::string& fqManagedClassName)
	{
		// TODO: insert return statement here
		Coral::Type& managedType = userAssembly.GetType(fqManagedClassName);
		Coral::ManagedObject managedObject = managedType.CreateInstance();

		managedObjects.push_back(managedObject);
		return managedObject;
	}
}