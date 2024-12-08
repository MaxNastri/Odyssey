#include "EditorWidgets.h"
#include "SceneManager.h"
#include "EditorComponents.h"
#include "Transform.h"
#include "imgui_internal.h"
#include "Input.h"

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

	SelectableInput::SelectableInput(std::string_view text, uint64_t iconHandle, float aspectRatio)
	{
		m_Text = text;
		m_IconHandle = iconHandle;
		m_AspectRatio = aspectRatio;
		m_Text.copy(m_InputBuffer, ARRAYSIZE(m_InputBuffer));
	}

	SelectableInput::Result SelectableInput::Draw()
	{
		bool previouslySelected = m_Selected;
		m_Result = Result::None;

		ImGui::PushID(this);

		if (!previouslySelected)
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, float4(0.0f));

		if (m_ShowInput)
		{
			DrawInput();
		}
		else
		{
			auto& style = ImGui::GetStyle();
			auto* window = ImGui::GetCurrentWindow();

			float textHeight = ImGui::CalcTextSize(m_Text.c_str()).y;
			float iconWidth = textHeight * m_AspectRatio;
			float startPosX = ImGui::GetCursorPosX();

			// Move the cursor along so we can draw the icon to the left of the selectable
			if (m_IconHandle)
				ImGui::SetCursorPosX(startPosX + iconWidth + style.ItemSpacing.x);

			const ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowDoubleClick |
				ImGuiSelectableFlags_SpanAllColumns |
				ImGuiSelectableFlags_AllowItemOverlap;

			// Extend the work rect to ignore window padding so the selectable highlight
			// fills the entire width
			window->ParentWorkRect.Min.x -= style.WindowPadding.x;
			window->ParentWorkRect.Max.x += style.WindowPadding.x;

			// Draw the selectable
			if (ImGui::Selectable(m_Text.c_str(), m_Selected, flags))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_Result = Result::DoubleClick;
				}
				else
				{
					m_Result = Result::Selected;
					m_Selected = true;
				}
			}

			// Check if its hovered
			bool hovered = ImGui::IsItemHovered();

			if (m_IconHandle)
			{
				// Move the cursor back to the left and draw the icon
				ImGui::SameLine();
				ImGui::SetCursorPosX(startPosX);
				ImGui::Image((void*)m_IconHandle, float2(iconWidth, textHeight));

				// Check if the icon is hovered
				hovered |= ImGui::IsItemHovered();
			}

			// Done drawing, restore the work rect to its original size
			window->ParentWorkRect.Min.x += style.WindowPadding.x;
			window->ParentWorkRect.Max.x -= style.WindowPadding.x;

			// We treat the icon and selectable as a single widget for hover checks
			if (!hovered && Input::GetMouseButtonDown(MouseButton::Left))
			{
				m_Selected = false;
			}
			else if (hovered && Input::GetMouseButtonDown(MouseButton::Right))
			{
				m_Selected = true;
				ImGui::OpenPopup("Popup");
			}

			if (ImGui::BeginPopup("Popup"))
			{
				if (ImGui::MenuItem("Rename"))
				{
					m_ShowInput = true;
				}

				ImGui::EndPopup();
			}
		}

		if (!previouslySelected)
			ImGui::PopStyleColor();

		ImGui::PopID();

		return m_Result;
	}

	void SelectableInput::SetIcon(uint64_t iconHandle, float aspectRatio)
	{
		m_IconHandle = iconHandle;
		m_AspectRatio = aspectRatio;
	}

	void SelectableInput::DrawInput()
	{
		if (m_IconHandle)
		{
			float textHeight = ImGui::CalcTextSize(m_Text.c_str()).y;
			float iconWidth = textHeight * m_AspectRatio;

			ImGui::Image((void*)m_IconHandle, float2(iconWidth, textHeight));
			ImGui::SameLine();
		}

		bool enter = ImGui::InputText("##Label", m_InputBuffer, ARRAYSIZE(m_InputBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
		
		ImGui::SetKeyboardFocusHere(-1);
		
		if (enter || (!ImGui::IsItemHovered() && Input::GetMouseButtonDown(MouseButton::Left)))
		{
			// Submit the text
			m_Text = m_InputBuffer;
			m_ShowInput = false;
			m_Result = Result::TextModified;
		}
	}

	bool SearchWidget::Draw()
	{
		ImGui::PushID(this);

		return false;
	}
}