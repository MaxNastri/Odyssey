#include "ScriptingManager.h"
#include <Logger.h>
#include "Events.h"
#include "Globals.h"
#include "ScriptCompiler.h"

namespace Odyssey
{
	void ExceptionCallback(std::string_view exception)
	{
		Logger::LogError(exception.data());
	}

	void ScriptingManager::Initialize()
	{
		// Initialize Coral
		std::filesystem::path appPath = Globals::GetApplicationPath();
		std::string coralDir = appPath.string();
		hostSettings =
		{
			.CoralDirectory = coralDir,
			.ExceptionCallback = ExceptionCallback
		};
		hostInstance.Initialize(hostSettings);
	}

	void ScriptingManager::LoadUserAssemblies()
	{
		if (!s_UserAssembliesLoaded)
		{
			userAssemblyContext = hostInstance.CreateAssemblyLoadContext("UserScripts");

			// Load the assembly
			userAssembly = userAssemblyContext.LoadAssembly(s_UserAssemblyPath.string());
			s_UserAssembliesLoaded = true;
		}
	}

	void ScriptingManager::UnloadUserAssemblies()
	{
		if (s_UserAssembliesLoaded)
		{
			for (Coral::ManagedObject& object : managedObjects)
			{
				object.Destroy();
			}
			managedObjects.clear();

			hostInstance.UnloadAssemblyLoadContext(userAssemblyContext);
			s_UserAssembliesLoaded = false;
		}
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