#include "AssetFieldDrawer.h"
#include "imgui.h"
#include "Logger.h"
#include "AssetManager.h"

namespace Odyssey
{
	AssetFieldDrawer::AssetFieldDrawer(const std::string& label, const std::string& guid, const std::string& assetType, std::function<void(const std::string&)> callback)
	{
		m_Label = label;
		m_GUID = guid;
		m_Type = assetType;
		m_OnValueModified = callback;

		if (!m_GUID.empty())
		{
			std::vector<std::string> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
			possibleGUIDs.insert(possibleGUIDs.begin(), "None");

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
	}

	void AssetFieldDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);

		std::vector<std::string> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
		possibleGUIDs.insert(possibleGUIDs.begin(), "None");

		ImGui::PushID((void*)this);

		std::string selectedDisplayName = selectedIndex != 0 ? AssetManager::GUIDToName(possibleGUIDs[selectedIndex]) : possibleGUIDs[selectedIndex];
		if (ImGui::BeginCombo("##Empty", selectedDisplayName.c_str()))
		{
			for (int32_t i = 0; i < possibleGUIDs.size(); i++)
			{
				const bool isSelected = selectedIndex == i;
				std::string displayName = i != 0 ? AssetManager::GUIDToName(possibleGUIDs[i]) : possibleGUIDs[i];
				
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
				m_GUID = (const char*)payload->Data;
				m_Modified = true;
				m_OnValueModified(m_GUID);
				Logger::LogInfo("(AssetFieldDrawer) Accepting D&D payload for mesh asset: " + m_GUID);
			}
			ImGui::EndDragDropTarget();
		}
	}
}