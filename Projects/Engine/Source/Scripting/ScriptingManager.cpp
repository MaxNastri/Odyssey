#include "ScriptingManager.h"
#include <Logger.h>
#include "Events.h"
#include "Globals.h"
#include "ScriptCompiler.h"
#include "ScriptBindings.h"
#include "Utils.h"
#include "GC.hpp"

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

		s_LoadContext = hostInstance.CreateAssemblyLoadContext("Odyssey.LoadContext");

		LoadFrameworkAssembly();
		ScriptBindings::Initialize(s_FrameworkAssembly);
	}

	void ScriptingManager::LoadFrameworkAssembly()
	{
		Path frameworkPath = std::filesystem::current_path() / "Resources/scripts/Odyssey.Framework.dll";

		s_FrameworkAssembly = s_LoadContext.LoadAssembly(frameworkPath.string());
		BuildScriptMetadata(s_FrameworkAssembly);
	}

	void ScriptingManager::LoadUserAssemblies()
	{
		if (!s_UserAssembliesLoaded)
		{
			// Load the assembly
			appAssembly = s_LoadContext.LoadAssembly(s_UserAssemblyPath.string());
			BuildScriptMetadata(appAssembly);

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
		for (auto& [scriptID, scriptMetadata] : m_ScriptMetdata)
		{
			for (auto& [fieldID, fieldMetadata] : scriptMetadata.Fields)
			{
				fieldMetadata.DefaultValue.Free();
			}
		}

		m_ScriptMetdata.clear();
		m_ManagedObjects.Clear();
		hostInstance.UnloadAssemblyLoadContext(s_LoadContext);
		s_UserAssembliesLoaded = false;
	}

	void ScriptingManager::Destroy()
	{
		Shutdown();
		hostInstance.Shutdown();
	}

	void ScriptingManager::AddEntityScript(GUID entityGUID, uint32_t scriptID)
	{
		if (!m_ScriptMetdata.contains(scriptID))
		{
			// TODO: Throw error
			return;
		}

		auto& metadata = GetScriptMetadata(scriptID);
		auto& scriptStorage = m_ScriptStorage[entityGUID];
		scriptStorage.ScriptID = scriptID;

		for (const auto& [fieldID, fieldMetadata] : metadata.Fields)
		{
			auto& fieldStorage = scriptStorage.Fields[fieldID];
			fieldStorage.Name = fieldMetadata.Name;
			fieldStorage.Type = fieldMetadata.ManagedType;
			fieldStorage.DataType = fieldMetadata.Type;

			if (!fieldMetadata.DefaultValue || fieldMetadata.DefaultValue.GetSize() == 0)
				fieldStorage.ValueBuffer.Allocate(DataTypeSize(fieldMetadata.Type));
			else
				fieldStorage.ValueBuffer = RawBuffer::Copy(fieldMetadata.DefaultValue);

			fieldStorage.Instance = nullptr;
		}
	}

	void ScriptingManager::BuildScriptMetadata(Coral::ManagedAssembly& assembly)
	{
		std::vector<Coral::Type*> types = assembly.GetTypes();
		Coral::Type& entityType = assembly.GetType("Odyssey.Entity");
		Coral::Type& componentType = assembly.GetType("Odyssey.Component");

		for (Coral::Type* type : types)
		{
			std::string fullName = type->GetFullName();
			uint32_t scriptID = GenerateFNVHash(fullName);

			if (type->IsSubclassOf(entityType))
			{
				ScriptMetadata& metadata = m_ScriptMetdata[scriptID];
				metadata.Name = fullName;
				metadata.Type = type;

				auto temp = type->CreateInstance();

				for (auto& fieldInfo : type->GetFields())
				{
					Coral::ScopedString fieldName = fieldInfo.GetName();
					std::string fieldNameStr = fieldName;
					Coral::Type* fieldType = &fieldInfo.GetType();

					if (fieldType->IsSZArray())
						fieldType = &fieldType->GetElementType();

					if (fieldInfo.GetAccessibility() != Coral::TypeAccessibility::Public)
						continue;

					Coral::ScopedString typeName = fieldType->GetFullName();
					std::string fullFieldName = std::format("{}.{}", fullName, fieldNameStr);
					uint32_t fieldID = GenerateFNVHash(fullFieldName);

					DataType dataType;

					// This is a known type
					if (g_DataTypeLookup.contains(typeName))
						dataType = g_DataTypeLookup.at(typeName);
					// Unknown type, but is a sub class of entity
					else if (fieldType->IsSubclassOf(entityType))
						dataType = DataType::Entity;
					// Unknown type, but is a sub class of component
					else if (fieldType->IsSubclassOf(componentType))
						dataType = DataType::Component;
					// Completely unknown, skip
					else
						continue;

					FieldMetadata& fieldMetadata = metadata.Fields[fieldID];
					fieldMetadata.Name = fieldName;
					fieldMetadata.Type = dataType;
					fieldMetadata.ManagedType = &fieldInfo.GetType();

					switch (fieldMetadata.Type)
					{
					case DataType::SByte:
						fieldMetadata.SetDefaultValue<int8_t>(temp);
						break;
					case DataType::Byte:
						fieldMetadata.SetDefaultValue<uint8_t>(temp);
						break;
					case DataType::Short:
						fieldMetadata.SetDefaultValue<int16_t>(temp);
						break;
					case DataType::UShort:
						fieldMetadata.SetDefaultValue<uint16_t>(temp);
						break;
					case DataType::Int:
						fieldMetadata.SetDefaultValue<int32_t>(temp);
						break;
					case DataType::UInt:
						fieldMetadata.SetDefaultValue<uint32_t>(temp);
						break;
					case DataType::Long:
						fieldMetadata.SetDefaultValue<int64_t>(temp);
						break;
					case DataType::ULong:
						fieldMetadata.SetDefaultValue<uint64_t>(temp);
						break;
					case DataType::Float:
						fieldMetadata.SetDefaultValue<float>(temp);
						break;
					case DataType::Double:
						fieldMetadata.SetDefaultValue<double>(temp);
						break;
					case DataType::Vector2:
						fieldMetadata.SetDefaultValue<glm::vec2>(temp);
						break;
					case DataType::Vector3:
						fieldMetadata.SetDefaultValue<glm::vec3>(temp);
						break;
					case DataType::Vector4:
						fieldMetadata.SetDefaultValue<glm::vec4>(temp);
						break;
					case DataType::Bool:
						fieldMetadata.SetDefaultValue<Coral::Bool32>(temp);
						break;
					case DataType::String:
					case DataType::Entity:
					case DataType::Mesh:
					case DataType::Material:
					case DataType::Texture2D:
					case DataType::Scene:
					case DataType::Component:
						break;
					default:
						break;
					}
				}

				temp.Destroy();
			}
		}
	}
	ScriptMetadata& ScriptingManager::GetScriptMetadata(uint32_t scriptID)
	{
		return m_ScriptMetdata.at(scriptID);
	}
	void ScriptingManager::DestroyInstance(GUID entityGUID)
	{
		ScriptStorage& scriptStorage = m_ScriptStorage[entityGUID];

		for (auto& [fieldID, fieldStorage] : scriptStorage.Fields)
			fieldStorage.Instance = nullptr;

		scriptStorage.Instance->Destroy();
		scriptStorage.Instance = nullptr;
	}

	ScriptStorage& ScriptingManager::GetScriptStorage(GUID guid)
	{
		return m_ScriptStorage[guid];
	}
}