#include "EditorWidgets.h"
#include "SceneManager.h"
#include "EditorComponents.h"
#include "Transform.h"

namespace Odyssey
{
	Dropdown::Dropdown(const std::vector<std::string>& options)
	{
		m_Options = options;
		m_SelectedIndex = 0;
	}

	Dropdown::Dropdown(const std::vector<std::string>& options, uint64_t initialIndex)
	{
		m_Options = options;
		m_SelectedIndex = initialIndex;
	}

	Dropdown::Dropdown(const std::vector<std::string>& options, std::string_view initialValue)
	{
		m_Options = options;
		m_SelectedIndex = 0;

		for (size_t i = 0; i < options.size(); i++)
		{
			if (options[i] == initialValue)
				m_SelectedIndex = i;
		}
	}

	bool Dropdown::Draw()
	{
		bool modified = false;

		std::string_view selectedOption = m_Options[m_SelectedIndex];

		ImGui::PushID(this);

		if (ImGui::BeginCombo("##Label", selectedOption.data()))
		{
			for (size_t i = 0; i < m_Options.size(); i++)
			{
				const bool isSelected = m_SelectedIndex == i;
				std::string_view name = m_Options[i];

				if (ImGui::Selectable(name.data(), isSelected))
				{
					m_SelectedIndex = i;
					modified = true;
					break;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();

		return modified;
	}

	void Dropdown::SetOptions(const std::vector<std::string>& options)
	{
		if (m_SelectedIndex != 0)
		{
			// Cache the previously selected option
			std::string selected = m_Options[m_SelectedIndex];

			// Reset the selected back to default
			m_SelectedIndex = 0;

			// Search the new options for the previously selected
			// We do this just in case the ordering has changed
			for (size_t i = 0; i < options.size(); i++)
			{
				if (options[i] == selected)
				{
					m_SelectedIndex = i;
					break;
				}
			}
		}

		m_Options = options;
	}

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