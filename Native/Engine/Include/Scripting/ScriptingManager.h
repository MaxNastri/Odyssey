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
		static void UnloadUserAssemblies();
		static void ReloadUserAssemblies();
		static void SetUserAssembliesPath(const Path& userAssemblyPath) { s_UserAssemblyPath = userAssemblyPath; }
		static void Unload();

	public:
		static Coral::ManagedObject CreateManagedObject(std::string_view fqManagedClassName);
	private:
		inline static Coral::HostInstance hostInstance;
		inline static Coral::HostSettings hostSettings;
		inline static Coral::AssemblyLoadContext s_FrameworkLoadContext;
		inline static Coral::AssemblyLoadContext s_AppLoadContext;
		inline static Coral::ManagedAssembly appAssembly;
		inline static Coral::ManagedAssembly s_FrameworkAssembly;
		inline static std::vector<Coral::ManagedObject> managedObjects;
		inline static bool s_UserAssembliesLoaded = false;
		inline static Path s_UserAssemblyPath;
	};
}