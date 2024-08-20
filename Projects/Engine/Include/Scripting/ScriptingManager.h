#pragma once
#include <HostInstance.hpp>
#include "ManagedObject.hpp"
#include "ScriptMetadata.h"
#include "ScriptStorage.h"
#include "GUID.h"
#include "ManagedHandle.h"

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
		static Coral::Type GetEntityType() { return s_FrameworkAssembly.GetType("Odyssey.Entity"); }
	
	public:
		static void AddEntityScript(GUID entityGUID, uint32_t scriptID);
		static void RemoveEntityScript(GUID entityGUID, uint32_t scriptID);
		static ScriptStorage& GetScriptStorage(GUID guid);
		static ScriptMetadata& GetScriptMetadata(uint32_t scriptID);
		static std::vector<ScriptMetadata> GetAllScriptMetadatas();
		static void DestroyInstance(GUID entityGUID);
	public:
		template<typename... Args>
		static ManagedHandle Instantiate(GUID entityGUID, Args&&... args)
		{
			ScriptStorage& scriptStorage = m_ScriptStorage[entityGUID];
			ScriptMetadata& metadata = m_ScriptMetdata[scriptStorage.ScriptID];

			Coral::ManagedObject instance = metadata.Type->CreateInstance(std::forward<Args>(args)...);
			auto [index, handle] = m_ManagedObjects.Insert(std::move(instance));

			scriptStorage.Instance = &handle;

			for (auto& [fieldID, fieldStorage] : scriptStorage.Fields)
			{
				const FieldMetadata& fieldMetadata = metadata.Fields[fieldID];

				// TODO CONVERT TO ATTRIBUTE CHECK
				if (fieldMetadata.Type == DataType::Entity || fieldMetadata.Type == DataType::Component)
				{
					GUID guid = 0;
					fieldStorage.TryGetValue<GUID>(guid);
					Coral::ManagedObject value = fieldMetadata.ManagedType->CreateInstance((uint64_t)guid);
					handle.SetFieldValue(fieldStorage.Name, value);
					value.Destroy();
				}
				else
				{
					if (fieldStorage.ValueBuffer.GetSize() > 0)
						handle.SetFieldValueRaw(fieldStorage.Name, fieldStorage.ValueBuffer.GetData());
				}

				fieldStorage.Instance = &handle;
			}

			return ManagedHandle(&handle);
		}

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
		inline static std::map<uint32_t, ScriptMetadata> m_ScriptMetdata;
		inline static std::map<GUID, ScriptStorage> m_ScriptStorage;
		inline static Coral::StableVector<Coral::ManagedObject> m_ManagedObjects;
	};
}