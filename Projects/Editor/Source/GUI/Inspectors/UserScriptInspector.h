#pragma once
#include "Inspector.h"
#include "PropertyDrawer.h"
#include "IntDrawer.h"
#include "GameObject.h"
#include "ManagedObject.hpp"
#include "ScriptingManager.h"
#include "ScriptStorage.h"

namespace Odyssey
{
	class PropertyDrawer;
	class ScriptComponent;

	class UserScriptInspector : public Inspector
	{
	public:
		UserScriptInspector() = default;
		UserScriptInspector(GameObject& go);

	public:
		virtual void Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(ScriptComponent* userScript);
		void CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, GUID initialValue);
		void CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage);
		void OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, const std::string& newValue);

	private:
		template<typename T>
		void AddIntDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
		{
			T initialValue = fieldStorage.GetValue<T>();
			auto drawer = std::make_shared<IntDrawer<T>>(fieldStorage.Name, initialValue,
				[this, scriptID, fieldID](T newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
			drawers.push_back(drawer);
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
		GameObject m_GameObject;
		std::string displayName;
		std::vector<std::shared_ptr<PropertyDrawer>> drawers;
	};
}