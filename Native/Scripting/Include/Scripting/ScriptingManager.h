#pragma once
#include "HostInstance.hpp"
#include "ManagedObject.hpp"
#include <EventSystem.h>

namespace Odyssey::Scripting
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
		static Coral::HostInstance hostInstance;
		static Coral::HostSettings hostSettings;
		static Coral::AssemblyLoadContext userAssemblyContext;
		static Coral::ManagedAssembly userAssembly;
		static std::vector<Coral::ManagedObject> managedObjects;
	private:
		inline static const std::string UserAssemblyFilename = "Odyssey.Managed.Example.dll";
	};
}