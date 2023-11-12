#pragma once
#include "HostInstance.hpp"
#include "ManagedObject.hpp"
#include "ScriptCompiler.h"
#include <EventSystem.h>

namespace Odyssey::Scripting
{
	class ScriptingManager
	{
	public:
		static void Initialize();
		static void LoadUserAssemblies();
		static void UnloadUserAssemblies();
		static bool RecompileUserAssemblies();
		static void ReloadUserAssemblies();

	public:
		static Coral::ManagedObject CreateManagedObject(const std::string& fqManagedClassName);
	private:
		static Coral::HostInstance hostInstance;
		static Coral::HostSettings hostSettings;
		static Coral::AssemblyLoadContext userAssemblyContext;
		static Coral::ManagedAssembly userAssembly;
		static ScriptCompiler scriptCompiler;
		static std::vector<Coral::ManagedObject> managedObjects;
	private:
		inline static const std::string UserAssemblyFilename = "Odyssey.Managed.Example.dll";
	};
}