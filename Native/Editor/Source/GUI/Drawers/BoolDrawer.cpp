#include "BoolDrawer.h"
#include <imgui.h>

namespace Odyssey
{
	BoolDrawer::BoolDrawer(const std::string& propertyLabel, bool initialValue, std::function<void(bool)> callback, bool readOnly)
		: PropertyDrawer(propertyLabel)
	{
		m_Data = initialValue;
		valueUpdatedCallback = callback;
		m_ReadOnly = readOnly;
	}

	void BoolDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);

			if (m_ReadOnly)
				ImGui::BeginDisabled();

			if (ImGui::Checkbox("##label", &m_Data))
			{
				valueUpdatedCallback(m_Data);
			}

			if (m_ReadOnly)
				ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}
}