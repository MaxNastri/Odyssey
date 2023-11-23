#include "ScriptingManager.h"
#include "Paths.h"
#include <Logger.h>
#include "Events.h"

namespace Odyssey
{
	Coral::HostInstance ScriptingManager::hostInstance;
	Coral::HostSettings ScriptingManager::hostSettings;
	Coral::AssemblyLoadContext ScriptingManager::userAssemblyContext;
	Coral::ManagedAssembly ScriptingManager::userAssembly;
	std::vector<Coral::ManagedObject> ScriptingManager::managedObjects;

	void ExceptionCallback(std::string_view exception)
	{
		Logger::LogError(exception.data());
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

	void ScriptingManager::ReloadUserAssemblies()
	{
		UnloadUserAssemblies();
		LoadUserAssemblies();
		EventSystem::Dispatch<OnAssembliesReloaded>();
	}

	Coral::ManagedObject ScriptingManager::CreateManagedObject(std::string_view fqManagedClassName)
	{
		// TODO: insert return statement here
		Coral::Type& managedType = userAssembly.GetType(fqManagedClassName);
		Coral::ManagedObject managedObject = managedType.CreateInstance();

		managedObjects.push_back(managedObject);
		return managedObject;
	}
}