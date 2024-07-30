#pragma once
#include <HostInstance.hpp>
#include "ManagedObject.hpp"
#include <EventSystem.h>

namespace Odyssey
{
	class ScriptingManager
	{
	public:
		static void Initialize();
		static void LoadUserAssemblies();
		static void UnloadUserAssemblies();
		static void ReloadUserAssemblies();

	public:
		static Coral::ManagedObject CreateManagedObject(std::string_view fqManagedClassName);
	private:
		inline static Coral::HostInstance hostInstance;
		inline static Coral::HostSettings hostSettings;
		inline static Coral::AssemblyLoadContext userAssemblyContext;
		inline static Coral::ManagedAssembly userAssembly;
		inline static std::vector<Coral::ManagedObject> managedObjects;
		inline static bool s_UserAssembliesLoaded = false;
	private:
		inline static const std::string UserAssemblyFilename = "Odyssey.Managed.Example.dll";
	};
}