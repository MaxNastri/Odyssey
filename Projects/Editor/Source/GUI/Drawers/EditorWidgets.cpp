#include "EditorWidgets.h"
#include "SceneManager.h"
#include "EditorComponents.h"
#include "Transform.h"

namespace Odyssey
{
	EntityDropdown::EntityDropdown(GUID initialValue, const std::string& typeName)
	{
		m_TypeName = typeName;
		m_SelectedIndex = 0;
		m_SelectedEntity = initialValue;
		GeneratePossibleEntities();
	}

	bool EntityDropdown::Draw()
	{
		bool modified = false;

		std::string selectedDisplayName = m_Entities[m_SelectedIndex].GameObjectName;

		ImGui::PushID(this);

		if (ImGui::BeginCombo("##Empty", selectedDisplayName.c_str()))
		{
			for (int32_t i = 0; i < m_Entities.size(); i++)
			{
				const bool isSelected = m_SelectedIndex == i;
				std::string displayName = m_Entities[i].GameObjectName;

				if (ImGui::Selectable(displayName.c_str(), isSelected))
				{
					m_SelectedIndex = i;
					m_SelectedEntity = m_Entities[m_SelectedIndex].GameObjectGUID;
					modified = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
			{
				uint64_t* payloadData = (uint64_t*)payload->Data;

				// Set the D&D payload as our GUID
				m_SelectedEntity = GUID(*payloadData);
				modified = true;

				GeneratePossibleEntities();
			}

			ImGui::EndDragDropTarget();
		}

		return modified;
	}

	void EntityDropdown::GeneratePossibleEntities()
	{
		Scene* scene = SceneManager::GetActiveScene();

		m_SelectedIndex = 0;
		m_Entities.clear();
		m_Entities.push_back({ "None", 0 });

		if (m_TypeName == Transform::Type)
		{
			for (auto entity : scene->GetAllEntitiesWith<Transform>())
			{
				GameObject gameObject = GameObject(scene, entity);

				// Skip any game objects marked to not show in the hierarchy
				if (EditorPropertiesComponent* properties = gameObject.TryGetComponent<EditorPropertiesComponent>())
					if (!properties->ShowInHierarchy)
						continue;

				m_Entities.push_back({ gameObject.GetName(), gameObject.GetGUID() });

				if (gameObject.GetGUID() == m_SelectedEntity)
					m_SelectedIndex = m_Entities.size() - 1;
			}
		}
	}
}