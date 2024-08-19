#include "UserScriptInspector.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "StringDrawer.h"
#include "ScriptComponent.h"
#include "imgui.h"
#include "ScriptingManager.h"
#include "EntityFieldDrawer.h"

namespace Odyssey
{
	UserScriptInspector::UserScriptInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ScriptComponent* scriptComponent = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			// TODO FIX THIS
			ScriptMetadata& metadata = ScriptingManager::GetScriptMetadata(scriptComponent->GetScriptID());
			displayName = metadata.Name;

			// Remove any namespaces
			size_t found = displayName.find_last_of('.');
			if (found != std::string::npos)
			{
				displayName = displayName.substr(found + 1);
			}

			InitializeDrawers(scriptComponent);
		}
	}

	void UserScriptInspector::Draw()
	{
		if (ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const auto& drawer : drawers)
				drawer->Draw();
		}
	}

	void UserScriptInspector::UpdateFields()
	{
		drawers.clear();
		if (ScriptComponent* userScript = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			InitializeDrawers(userScript);
		}
	}

	void UserScriptInspector::InitializeDrawers(ScriptComponent* userScript)
	{
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::Entity)
			{
				GUID initialValue = 0;
				fieldStorage.TryGetValue(initialValue);
				CreateEntityDrawer(fieldStorage.Name, storage.ScriptID, fieldID, initialValue);
			}
			else if (fieldStorage.DataType == DataType::String)
				CreateStringDrawer(storage.ScriptID, fieldID, fieldStorage);
			else
				CreateDrawerFromProperty(storage.ScriptID, fieldID, fieldStorage);
		}
	}

	void UserScriptInspector::CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, GUID initialValue)
	{
		auto drawer = std::make_shared<EntityFieldDrawer>(fieldName, initialValue,
			[this, scriptID, fieldID](GUID guid) { OnFieldChanged<GUID>(scriptID, fieldID, guid); });
		drawers.push_back(drawer);
	}

	template<typename T>
	std::shared_ptr<IntDrawer<T>> CreateIntDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		T initialValue = fieldStorage.GetValue<T>();
		auto drawer = std::make_shared<IntDrawer<T>>(fieldStorage.Name, initialValue,
			[this](T newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
		return drawer;
	}

	void UserScriptInspector::CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		switch (fieldStorage.DataType)
		{
			case DataType::Byte:
			{
				AddIntDrawer<uint8_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::UShort:
			{
				AddIntDrawer<uint16_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::UInt:
			{
				AddIntDrawer<uint32_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::ULong:
			{
				AddIntDrawer<uint64_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::SByte:
			{
				AddIntDrawer<char8_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::Short:
			{
				AddIntDrawer<int16_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::Int:
			{
				AddIntDrawer<int32_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::Long:
			{
				AddIntDrawer<int64_t>(scriptID, fieldID, fieldStorage);
				break;
			}
			case DataType::Float:
			{
				float initialValue = fieldStorage.GetValue<float>();
				auto drawer = std::make_shared<FloatDrawer>(fieldStorage.Name, initialValue,
					[this, scriptID, fieldID](float newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
				drawers.push_back(drawer);
				break;
			}
			case DataType::Double:
			{
				double initialValue = fieldStorage.GetValue<double>();
				auto drawer = std::make_shared<DoubleDrawer>(fieldStorage.Name, initialValue,
					[this, scriptID, fieldID](double newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
				drawers.push_back(drawer);
				break;
			}
			case DataType::Bool:
			{
				Coral::Bool32 initialValue = fieldStorage.GetValue<Coral::Bool32>();
				auto drawer = std::make_shared<BoolDrawer>(fieldStorage.Name, initialValue,
					[this, scriptID, fieldID](bool newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
				drawers.push_back(drawer);
				break;
			}
		}
	}

	void UserScriptInspector::CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		std::string initialValue = "";
		Coral::String storedValue;

		// Check if the field storage has a valid string before assigning it
		if (fieldStorage.TryGetValue<Coral::String>(storedValue))
		{
			initialValue = storedValue;
		}

		auto drawer = std::make_shared<StringDrawer>(fieldStorage.Name, initialValue,
			[this, scriptID, fieldID](const std::string& newValue) { OnStringFieldChanged(scriptID, fieldID, newValue); });
		drawers.push_back(drawer);
	}

	void UserScriptInspector::OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, const std::string& newValue)
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
				fieldStorage.ValueBuffer.Allocate(newValue.size());
				fieldStorage.SetValue(newValue);
				break;
			}
		}
	}
}