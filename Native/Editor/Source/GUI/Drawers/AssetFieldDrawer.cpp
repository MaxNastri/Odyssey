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
			// Find this guid's selected 
			std::vector<std::string> possibleGUIDs = AssetManager::GetAssetsOfType(m_Type);
			possibleGUIDs.insert(possibleGUIDs.begin(), "None");

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
		if (ImGui::BeginCombo("", possibleGUIDs[selectedIndex].c_str()))
		{
			for (int32_t i = 0; i < possibleGUIDs.size(); i++)
			{
				const bool isSelected = selectedIndex == i;

				if (ImGui::Selectable(possibleGUIDs[i].c_str(), isSelected) && i != 0)
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