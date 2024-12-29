#include "PropertyDrawers.h"
#include "imgui_internal.h"
#include "AssetManager.h"
#include "Log.h"
#include "SceneManager.h"
#include "Transform.h"
#include "EditorComponents.h"

namespace Odyssey
{
	AssetFieldDrawer::AssetFieldDrawer(std::string_view label, GUID guid, const std::string& assetType, std::function<void(GUID)> callback)
		: PropertyDrawer(label)
	{
		m_GUID = guid;
		m_Type = assetType;
		m_OnValueModified = callback;
		SetSelectedIndex();
	}

	bool AssetFieldDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			std::vector<GUID> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
			possibleGUIDs.insert(possibleGUIDs.begin(), 0);


			std::string selectedDisplayName = selectedIndex != 0 ? AssetManager::GUIDToName(possibleGUIDs[selectedIndex]) : "None";
			if (ImGui::BeginCombo("##Empty", selectedDisplayName.c_str()))
			{
				for (int32_t i = 0; i < possibleGUIDs.size(); i++)
				{
					const bool isSelected = selectedIndex == i;
					std::string displayName = i != 0 ? AssetManager::GUIDToName(possibleGUIDs[i]) : "None";

					if (ImGui::Selectable(displayName.c_str(), isSelected))
					{
						selectedIndex = i;
						m_GUID = possibleGUIDs[selectedIndex];

						modified = true;
						if (m_OnValueModified)
							m_OnValueModified(m_GUID);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
				{
					m_GUID = *((GUID*)payload->Data);

					modified = true;
					if (m_OnValueModified)
						m_OnValueModified(m_GUID);

					Log::Info("(AssetFieldDrawer) Accepting D&D payload for mesh asset: " + m_GUID.String());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	void AssetFieldDrawer::SetGUID(GUID guid)
	{
		m_GUID = guid;
		SetSelectedIndex();
	}

	void AssetFieldDrawer::SetSelectedIndex()
	{
		std::vector<GUID> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
		possibleGUIDs.insert(possibleGUIDs.begin(), 0);

		// Find this guid's selected index
		for (uint32_t i = 0; i < possibleGUIDs.size(); i++)
		{
			if (possibleGUIDs[i] == m_GUID)
			{
				selectedIndex = i;
				break;
			}
		}
	}
	BoolDrawer::BoolDrawer(std::string_view label, bool initialValue, bool readOnly, std::function<void(bool)> callback)
		: PropertyDrawer(label)
	{
		m_Value = initialValue;
		m_OnValueModifed = callback;
		m_ReadOnly = readOnly;
	}

	bool BoolDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("##Table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_ReadOnly)
				ImGui::BeginDisabled();

			if (ImGui::Checkbox("##label", &m_Value))
			{
				modified = true;

				if (m_OnValueModifed)
					m_OnValueModifed(m_Value);
			}

			if (m_ReadOnly)
				ImGui::EndDisabled();

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	ColorPicker::ColorPicker(std::string_view propertyLabel, float3 initialValue, std::function<void(float3)> callback)
	{
		m_Label = propertyLabel;
		m_Color = float4(initialValue, 1.0f);
		m_OnColor3Modified = callback;
		m_ColorType = ColorType::Color3;
	}

	ColorPicker::ColorPicker(std::string_view propertyLabel, float4 initialValue, std::function<void(float4)> callback)
	{
		m_Label = propertyLabel;
		m_Color = initialValue;
		m_OnColor4Modified = callback;
		m_ColorType = ColorType::Color4;
	}

	bool ColorPicker::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_ColorType == ColorType::Color3)
				modified = ImGui::ColorEdit3(m_Label.data(), glm::value_ptr(m_Color));
			else
				modified = ImGui::ColorEdit4(m_Label.data(), glm::value_ptr(m_Color));

			if (modified)
			{
				if (m_OnColor3Modified)
					m_OnColor3Modified(m_Color);
				else if (m_OnColor4Modified)
					m_OnColor4Modified(m_Color);
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	DoubleDrawer::DoubleDrawer(std::string_view label, double initialValue, std::function<void(double)> callback)
		: PropertyDrawer(label)
	{
		m_Value = initialValue;
		m_OnValueModifed = callback;
	}

	bool DoubleDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (ImGui::InputDouble(m_Label.data(), &m_Value, m_Step, m_StepFast))
			{
				modified = true;

				if (m_OnValueModifed)
					m_OnValueModifed(m_Value);
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	DropdownDrawer::DropdownDrawer(std::string_view label, const std::vector<std::string>& options, uint64_t startIndex, std::function<void(std::string_view, uint64_t)> callback)
		: m_Dropdown(options, startIndex)
	{
		m_Label = label;
		m_OnValueModified = callback;
	}

	DropdownDrawer::DropdownDrawer(std::string_view label, const std::vector<std::string>& options, std::string_view initialValue, std::function<void(std::string_view, uint64_t)> callback)
		: m_Dropdown(options, initialValue)
	{
		m_Label = label;
		m_OnValueModified = callback;
	}

	bool DropdownDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_Dropdown.Draw())
			{
				modified = true;

				if (m_OnValueModified)
					m_OnValueModified(m_Dropdown.GetSelectedString(), m_Dropdown.GetSelectedIndex());
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	std::string_view DropdownDrawer::GetSelected()
	{
		return m_Dropdown.GetSelectedString();
	}

	uint64_t DropdownDrawer::GetSelectedIndex()
	{
		return m_Dropdown.GetSelectedIndex();
	}

	void DropdownDrawer::SetOptions(const std::vector<std::string>& options)
	{
		m_Dropdown.SetOptions(options);
	}

	EntityFieldDrawer::EntityFieldDrawer(std::string_view label, GUID guid, const std::string& typeName, std::function<void(GUID)> callback)
		: m_Dropdown(guid, typeName)
	{
		m_Label = label;
		m_OnValueModified = callback;
	}

	bool EntityFieldDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);

			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_Dropdown.Draw())
			{
				modified = true;

				if (m_OnValueModified)
					m_OnValueModified(m_Dropdown.GetEntity());
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	FloatDrawer::FloatDrawer(std::string_view label, float initialValue, std::function<void(float)> callback)
		: PropertyDrawer(label)
	{
		m_Value = initialValue;
		m_OnValueModifed = callback;
	}

	bool FloatDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (ImGui::InputFloat(m_Label.data(), &m_Value, m_Step, m_StepFast))
			{
				modified = true;

				if (m_OnValueModifed)
					m_OnValueModifed(m_Value);
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	RangeSlider::RangeSlider(const std::string& label, float2 range, float2 limits, float step, bool drawMinMaxLabels, std::function<void(float2)> callback)
		: PropertyDrawer(label)
	{
		m_Range = range;
		m_Limits = limits;
		m_Step = step;
		m_OnValueModifed = callback;
		if (drawMinMaxLabels)
		{
			m_MinLabel = "Min: %.2f";
			m_MaxLabel = "Max: %.2f";
		}
		else
		{
			m_MinLabel = "%.2f";
			m_MaxLabel = "%.2f";
		}
	}

	bool RangeSlider::Draw()
	{
		bool modified = false;
		ImGui::PushID(this);

		if (ImGui::BeginTable("#RangeSlider", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::DragFloatRange2(m_Label.data(), &m_Range.x, &m_Range.y, m_Step, m_Limits.x, m_Limits.y,
				m_MinLabel.c_str(), m_MaxLabel.c_str(), ImGuiSliderFlags_AlwaysClamp))
			{
				modified = true;
				if (m_OnValueModifed)
					m_OnValueModifed(m_Range);
			}
			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	StringDrawer::StringDrawer(std::string_view label, std::string_view initialValue, bool readOnly, std::function<void(std::string_view)> callback)
		: PropertyDrawer(label)
	{
		initialValue.copy(m_Value, ARRAYSIZE(m_Value));
		m_OnValueModifed = callback;
		m_ReadOnly = readOnly;
	}

	bool StringDrawer::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_ReadOnly)
				ImGui::BeginDisabled();

			if (ImGui::InputText(m_Label.data(), m_Value, IM_ARRAYSIZE(m_Value)))
			{
				modified = true;

				if (m_OnValueModifed)
					m_OnValueModifed(m_Value);
			}

			if (m_ReadOnly)
				ImGui::EndDisabled();

			ImGui::EndTable();
		}

		ImGui::PopID();

		return modified;
	}

	Vector3Drawer::Vector3Drawer(std::string_view propertyLabel, glm::vec3 initialValue, glm::vec3 resetValue, bool drawButtons, std::function<void(glm::vec3)> callback)
	{
		m_Label = propertyLabel;
		m_Value = initialValue;
		m_ResetValue = resetValue;
		m_DrawButtons = drawButtons;
		onValueModified = callback;
	}

	bool Vector3Drawer::Draw()
	{
		bool modified = false;
		ImGui::PushID(this);

		if (ImGui::BeginTable("##Table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::PushID((void*)this);
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::Text(m_Label.c_str());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			uint8_t itemWidth = m_DrawButtons ? 4 : 3;
			ImVec2 spacing = m_DrawButtons ? ImVec2{ 0,0 } : ImVec2{ 5, 0 };
			ImGui::PushMultiItemsWidths(itemWidth, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

			float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 5.0f, lineHeight };

			{
				if (m_DrawButtons)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

					if (ImGui::Button("X", buttonSize))
					{
						m_Value.x = m_ResetValue.x;
						modified = true;
					}
					ImGui::SameLine();
				}

				modified |= ImGui::DragFloat("##x", &m_Value.x, 0.1f, 0.0f, 0.0f, "%.3f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				if (m_DrawButtons)
					ImGui::PopStyleColor(3);
			}

			{
				if (m_DrawButtons)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
					if (ImGui::Button("Y", buttonSize))
					{
						m_Value.y = m_ResetValue.y;
						modified = true;
					}
					ImGui::SameLine();
				}

				modified |= ImGui::DragFloat("##y", &m_Value.y, 0.1f, 0.0f, 0.0f, "%.3f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				if (m_DrawButtons)
					ImGui::PopStyleColor(3);
			}

			{
				if (m_DrawButtons)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
					if (ImGui::Button("Z", buttonSize))
					{
						m_Value.z = m_ResetValue.z;
						modified = true;
					}
					ImGui::SameLine();
				}
				modified |= ImGui::DragFloat("##z", &m_Value.z, 0.1f, 0.0f, 0.0f, "%.3f");
				ImGui::PopItemWidth();

				if (m_DrawButtons)
					ImGui::PopStyleColor(3);

			}

			ImGui::PopStyleVar();
			ImGui::PopID();

			if (modified)
				onValueModified(m_Value);

			ImGui::EndTable();
		}

		ImGui::PopID();
		return modified;
	}
}