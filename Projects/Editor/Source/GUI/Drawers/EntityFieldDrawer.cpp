#include "EntityFieldDrawer.h"
#include "imgui.h"
#include "Logger.h"

namespace Odyssey
{
    EntityFieldDrawer::EntityFieldDrawer(std::string_view label, GUID guid, std::function<void(GUID)> callback)
    {
		m_Label = label;
		m_GUID = guid;
		m_OnValueModified = callback;
		GeneratePossibleGUIDs();
    }

    void EntityFieldDrawer::Draw()
    {
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			ImGui::PushID((void*)this);

			std::string selectedDisplayName = m_SelectedIndex != 0 ? m_PossibleGUIDs[m_SelectedIndex].String() : "None";
			if (ImGui::BeginCombo("##Empty", selectedDisplayName.c_str()))
			{
				for (int32_t i = 0; i < m_PossibleGUIDs.size(); i++)
				{
					const bool isSelected = m_SelectedIndex == i;
					std::string displayName = i != 0 ? m_PossibleGUIDs[i].String() : "None";

					if (ImGui::Selectable(displayName.c_str(), isSelected) && i != 0)
					{
						m_SelectedIndex = i;
						m_OnValueModified(m_PossibleGUIDs[m_SelectedIndex]);
						m_Modified = true;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::PopID();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
				{
					uint64_t* payloadData = (uint64_t*)payload->Data;
					m_GUID = GUID(*payloadData);
					m_Modified = true;
					GeneratePossibleGUIDs();
					m_OnValueModified(m_GUID);
					Logger::LogInfo("(AssetFieldDrawer) Accepting Entity payload: " + m_GUID.String());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::EndTable();
		}
    }

	void EntityFieldDrawer::GeneratePossibleGUIDs()
	{
		m_PossibleGUIDs = { 0, m_GUID };

		for (size_t i = 0; i < m_PossibleGUIDs.size(); i++)
		{
			if (m_PossibleGUIDs[i] == m_GUID)
				m_SelectedIndex = i;
		}
	}
}