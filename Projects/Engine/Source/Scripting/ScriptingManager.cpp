#include "ScriptingManager.h"
#include <Logger.h>
#include "Events.h"
#include "Globals.h"
#include "ScriptCompiler.h"
#include "ScriptBindings.h"

namespace Odyssey
{
	void ExceptionCallback(std::string_view exception)
	{
		Logger::LogError(exception.data());
	}

	void ScriptingManager::Initialize()
	{
		// Initialize Coral
		if (!s_HostInitialized)
		{
			std::filesystem::path appPath = Globals::GetApplicationPath();
			std::string coralDir = appPath.string();
			hostSettings =
			{
				.CoralDirectory = coralDir,
				.ExceptionCallback = ExceptionCallback
			};
			hostInstance.Initialize(hostSettings);

			s_HostInitialized = true;
		}

		LoadFrameworkAssembly();

		ScriptBindings::Initialize(s_FrameworkAssembly);
	}

	void ScriptingManager::LoadFrameworkAssembly()
	{
		s_LoadContext = hostInstance.CreateAssemblyLoadContext("Odyssey.FrameworkContext");

		Path frameworkPath = std::filesystem::current_path() / "Resources/scripts/Odyssey.Framework.dll";
		s_FrameworkAssembly = s_LoadContext.LoadAssembly(frameworkPath.string());
	}

	void ScriptingManager::LoadUserAssemblies()
	{
		if (!s_UserAssembliesLoaded)
		{
			// Load the assembly
			appAssembly = s_LoadContext.LoadAssembly(s_UserAssemblyPath.string());
			s_UserAssembliesLoaded = true;
		}
	}

	void ScriptingManager::ReloadAssemblies()
	{
		Shutdown();
		Initialize();
		LoadUserAssemblies();
		EventSystem::Dispatch<OnAssembliesReloaded>();
	}

	void ScriptingManager::Shutdown()
	{
		for (Coral::ManagedObject& object : managedObjects)
		{
			object.Destroy();
		}
		managedObjects.clear();

		hostInstance.UnloadAssemblyLoadContext(s_LoadContext);
		s_UserAssembliesLoaded = false;
	}

	void ScriptingManager::Destroy()
	{
		Shutdown();
		hostInstance.Shutdown();
	}

	Coral::ManagedObject ScriptingManager::CreateManagedObject(std::string_view fqManagedClassName, uint64_t entityGUID)
	{
		Coral::Type& managedType = appAssembly.GetType(fqManagedClassName);
		Coral::ManagedObject managedObject = managedType.CreateInstance((uint64_t)entityGUID);

		managedObjects.push_back(managedObject);
		return managedObject;
	}
}