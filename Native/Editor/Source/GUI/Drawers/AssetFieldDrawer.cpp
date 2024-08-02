#include "AssetFieldDrawer.h"
#include "imgui.h"
#include "Logger.h"
#include "AssetManager.h"

namespace Odyssey
{
	AssetFieldDrawer::AssetFieldDrawer(const std::string& label, GUID guid, const std::string& assetType, std::function<void(GUID)> callback)
	{
		m_Label = label;
		m_GUID = guid;
		m_Type = assetType;
		m_OnValueModified = callback;

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

	void AssetFieldDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			std::vector<GUID> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
			possibleGUIDs.insert(possibleGUIDs.begin(), 0);

			ImGui::PushID((void*)this);

			std::string selectedDisplayName = selectedIndex != 0 ? AssetManager::GUIDToName(possibleGUIDs[selectedIndex]) : "None";
			if (ImGui::BeginCombo("##Empty", selectedDisplayName.c_str()))
			{
				for (int32_t i = 0; i < possibleGUIDs.size(); i++)
				{
					const bool isSelected = selectedIndex == i;
					std::string displayName = i != 0 ? AssetManager::GUIDToName(possibleGUIDs[i]) : "None";

					if (ImGui::Selectable(displayName.c_str(), isSelected) && i != 0)
					{
						selectedIndex = i;
						m_OnValueModified(possibleGUIDs[selectedIndex]);
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
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
				{
					std::string guid((const char*)payload->Data);
					m_GUID = GUID(guid);
					m_Modified = true;
					m_OnValueModified(m_GUID);
					Logger::LogInfo("(AssetFieldDrawer) Accepting D&D payload for mesh asset: " + m_GUID);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::EndTable();
		}
	}
}