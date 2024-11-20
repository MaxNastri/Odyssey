#pragma once
#include "Inspector.h"
#include "PropertyDrawers.h"
#include "GameObject.h"
#include "ManagedObject.hpp"
#include "ScriptingManager.h"
#include "ScriptStorage.h"

namespace Odyssey
{
	class PropertyDrawer;
	class ScriptComponent;

	class ScriptInspector : public Inspector
	{
	public:
		ScriptInspector() = default;
		ScriptInspector(GameObject& go);

	public:
		virtual bool Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(ScriptComponent* userScript);
		void CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, const std::string& typeName, GUID initialValue);
		void CreateAssetDrawer(const std::string& fieldName, const std::string& assetType, uint32_t scriptID, uint32_t fieldID, GUID initialValue);
		void CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, std::string_view newValue);

	private:
		template<typename T>
		void AddIntDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
		{
			T initialValue = fieldStorage.GetValue<T>();
			auto drawer = std::make_shared<IntDrawer<T>>(fieldStorage.Name, initialValue,
				[this, scriptID, fieldID](T newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
			drawers.push_back(drawer);
		}

		void OnEntityChanged(uint32_t scriptID, uint32_t fieldID, GUID newValue)
		{
			auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

			// Validate we are working on the same script
			if (storage.ScriptID != scriptID)
				return;

			// Look through the field storage for the matching field
			for (auto& [storedFieldID, fieldStorage] : storage.Fields)
			{
				if (fieldID == storedFieldID)
				{
					fieldStorage.SetValue<GUID>(newValue);
					if (newValue == 0 && fieldStorage.Instance)
					{
						fieldStorage.Instance->Destroy();
					}
					break;
				}
			}
		}

		template<typename T>
		void OnFieldChanged(uint32_t scriptID, uint32_t fieldID, T newValue)
		{
			auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

			// Validate we are working on the same script
			if (storage.ScriptID != scriptID)
				return;

			// Look through the field storage for the matching field
			for (auto& [storedFieldID, fieldStorage] : storage.Fields)
			{
				if (fieldID == storedFieldID)
				{
					fieldStorage.SetValue<T>(newValue);
					break;
				}
			}
		}
	private:
		bool m_ScriptEnabled;
		GameObject m_GameObject;
		std::string displayName;
		std::vector<std::shared_ptr<PropertyDrawer>> drawers;
	};
}