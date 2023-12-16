#include "AssetFieldDrawer.h"
#include "imgui.h"
#include "Logger.h"

namespace Odyssey
{
	AssetFieldDrawer::AssetFieldDrawer(const std::string& label, const std::string& guid, std::function<void(const std::string&)> callback)
	{
		m_Label = label;
		m_GUID = guid;
		m_OnValueModified = callback;
	}

	void AssetFieldDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);

		if (ImGui::Selectable(m_GUID.c_str(), false))
		{
			// On value modified
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
			{
				m_GUID = (const char*)payload->Data;
				m_OnValueModified(m_GUID);
				Logger::LogInfo("(AssetFieldDrawer) Accepting D&D payload for mesh asset: " + m_GUID);
			}
			ImGui::EndDragDropTarget();
		}
	}
}