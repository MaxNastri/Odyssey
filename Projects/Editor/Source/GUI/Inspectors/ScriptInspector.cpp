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
			for (Ref<PropertyDrawer>& drawer : drawers)
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
		auto callback = [this, scriptID, fieldID](GUID guid)
			{
				OnFieldChanged<GUID>(scriptID, fieldID, guid);
			};

		drawers.emplace_back(new EntityFieldDrawer(fieldName, initialValue, typeName, callback));
	}

	void ScriptInspector::CreateAssetDrawer(const std::string& fieldName, const std::string& assetType, uint32_t scriptID, uint32_t fieldID, GUID initialValue)
	{
		auto callback = [this, scriptID, fieldID](GUID guid)
			{
				OnFieldChanged<GUID>(scriptID, fieldID, guid);
			};

		drawers.emplace_back(new AssetFieldDrawer(fieldName, initialValue, assetType, callback));
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
				auto callback = [this, scriptID, fieldID](float newValue)
					{
						OnFieldChanged(scriptID, fieldID, newValue);
					};

				drawers.emplace_back(new FloatDrawer(fieldStorage.Name, initialValue, callback));
				break;
			}
			case DataType::Double:
			{
				double initialValue = fieldStorage.GetValue<double>();
				auto callback = [this, scriptID, fieldID](double newValue)
					{
						OnFieldChanged(scriptID, fieldID, newValue);
					};

				drawers.emplace_back(new DoubleDrawer(fieldStorage.Name, initialValue, callback));
				break;
			}
			case DataType::Bool:
			{
				Coral::Bool32 initialValue = fieldStorage.GetValue<Coral::Bool32>();
				auto callback = [this, scriptID, fieldID](bool newValue)
					{
						OnFieldChanged(scriptID, fieldID, newValue);
					};

				drawers.emplace_back(new BoolDrawer(fieldStorage.Name, initialValue, false, callback));
				break;
			}
			case DataType::Vector3:
			{
				glm::vec3 initialValue = fieldStorage.GetValue<glm::vec3>();
				auto callback = [this, scriptID, fieldID](glm::vec3 newValue)
					{
						OnFieldChanged(scriptID, fieldID, newValue);
					};

				drawers.emplace_back(new Vector3Drawer(fieldStorage.Name, initialValue, glm::zero<glm::vec3>(), false, callback));
				break;
			}
		}
	}

	void ScriptInspector::CreateStringDrawer(uint32_t scriptID, uint32_t fieldID, FieldStorage& fieldStorage)
	{
		auto callback = [this, scriptID, fieldID](std::string_view newValue)
			{
				OnStringFieldChanged(scriptID, fieldID, newValue);
			};

		drawers.emplace_back(new StringDrawer(fieldStorage.Name, "", false, callback));
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