#pragma once
#include <HostInstance.hpp>
#include "ManagedObject.hpp"

namespace Odyssey
{
	class ScriptingManager
	{
	public:
		static void Initialize();
		static void LoadFrameworkAssembly();
		static void LoadUserAssemblies();
		static void ReloadAssemblies();
		static void SetUserAssembliesPath(const Path& userAssemblyPath) { s_UserAssemblyPath = userAssemblyPath; }
		static void Shutdown();
		static void Destroy();

	public:
		static Coral::ManagedObject CreateManagedObject(std::string_view fqManagedClassName, uint64_t entityGUID);
		static Coral::Type GetEntityType() { return s_FrameworkAssembly.GetType("Odyssey.Entity"); }
	
	private: // Host settings
		inline static Coral::HostInstance hostInstance;
		inline static Coral::HostSettings hostSettings;
		inline static bool s_HostInitialized = false;

		// Assembly loading
		inline static Coral::AssemblyLoadContext s_LoadContext;
		inline static Coral::ManagedAssembly appAssembly;
		inline static Coral::ManagedAssembly s_FrameworkAssembly;
		inline static bool s_UserAssembliesLoaded = false;
		inline static Path s_UserAssemblyPath;

		inline static std::vector<Coral::ManagedObject> managedObjects;
	};
}