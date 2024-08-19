#pragma once
#include <HostInstance.hpp>
#include "ManagedObject.hpp"
#include "ScriptMetadata.h"
#include "ScriptStorage.h"
#include "GUID.h"

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
	
	public:
		static void AddEntityScript(GUID entityGUID, uint32_t scriptID);
		static ScriptStorage& GetScriptStorage(GUID guid);
		static ScriptMetadata& GetScriptMetadata(uint32_t scriptID);

	private:
		static void BuildScriptMetadata(Coral::ManagedAssembly& assembly);

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

		// Script management
		inline static std::map<GUID, ScriptMetadata> m_ScriptMetdata;
		inline static std::map<GUID, ScriptStorage> m_ScriptStorage;
		inline static std::vector<Coral::ManagedObject> managedObjects;
	};
}