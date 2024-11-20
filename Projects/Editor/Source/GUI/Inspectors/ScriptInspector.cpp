#include "ScriptInspector.h"
#include "PropertyDrawers.h"
#include "ScriptComponent.h"
#include "imgui.h"
#include "ScriptingManager.h"

namespace Odyssey
{
	ScriptInspector::ScriptInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ScriptComponent* scriptComponent = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			m_ScriptEnabled = scriptComponent->IsEnabled();

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

	bool ScriptInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_ScriptEnabled))
		{
			if (ScriptComponent* script = m_GameObject.TryGetComponent<ScriptComponent>())
				script->SetEnabled(m_ScriptEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader(("Script - " + displayName).c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const auto& drawer : drawers)
				modified |= drawer->Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void ScriptInspector::UpdateFields()
	{
		drawers.clear();
		if (ScriptComponent* script = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			InitializeDrawers(script);
		}
	}

	void ScriptInspector::InitializeDrawers(ScriptComponent* userScript)
	{
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::Entity || fieldStorage.DataType == DataType::Component)
			{
				Coral::ScopedString typeName = fieldStorage.Type->GetFullName();
				GUID initialValue;
				fieldStorage.TryGetValue(initialValue);

				CreateEntityDrawer(fieldStorage.Name, storage.ScriptID, fieldID, typeName, initialValue);
			}
			// TODO: Convert into IsAssetType check
			else if (fieldStorage.DataType == DataType::Mesh || fieldStorage.DataType == DataType::Material)
			{
				GUID initialValue;
				fieldStorage.TryGetValue(initialValue);
				CreateAssetDrawer(fieldStorage.Name, fieldStorage.Type->GetFullName(), storage.ScriptID, fieldID, initialValue);
			}
			else if (fieldStorage.DataType == DataType::String)
				CreateStringDrawer(storage.ScriptID, fieldID, fieldStorage);
			else
				CreateDrawerFromProperty(storage.ScriptID, fieldID, fieldStorage);
		}
	}

	void ScriptInspector::CreateEntityDrawer(std::string_view fieldName, uint32_t scriptID, uint32_t fieldID, const std::string& typeName, GUID initialValue)
	{
		auto drawer = std::make_shared<EntityFieldDrawer>(fieldName, initialValue, typeName,
			[this, scriptID, fieldID](GUID guid) { OnFieldChanged<GUID>(scriptID, fieldID, guid); });
		drawers.push_back(drawer);
	}

	void ScriptInspector::CreateAssetDrawer(const std::string& fieldName, const std::string& assetType, uint32_t scriptID, uint32_t fieldID, GUID initialValue)
	{
		auto drawer = std::make_shared<AssetFieldDrawer>(fieldName, initialValue, assetType,
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

	void ScriptInspector::CreateDrawerFromProperty(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
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
			case DataType::Vector3:
			{
				glm::vec3 initialValue = fieldStorage.GetValue<glm::vec3>();
				auto drawer = std::make_shared<Vector3Drawer>(fieldStorage.Name, initialValue, glm::zero<glm::vec3>(), false,
					[this, scriptID, fieldID](glm::vec3 newValue) { OnFieldChanged(scriptID, fieldID, newValue); });
				drawers.push_back(drawer);
				break;
			}
		}
	}

	void ScriptInspector::CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		std::string initialValue = "";

		auto drawer = std::make_shared<StringDrawer>(fieldStorage.Name, initialValue,
			[this, scriptID, fieldID](std::string_view newValue) { OnStringFieldChanged(scriptID, fieldID, newValue); });
		drawers.push_back(drawer);
	}

	void ScriptInspector::OnStringFieldChanged(uint32_t scriptID, uint32_t fieldID, std::string_view newValue)
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