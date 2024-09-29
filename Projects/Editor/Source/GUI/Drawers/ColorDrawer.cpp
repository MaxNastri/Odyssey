#include "ColorDrawer.h"
#include "imgui.h"

namespace Odyssey
{
	ColorDrawer::ColorDrawer(std::string_view propertyLabel, glm::vec3 initialValue, std::function<void(glm::vec3)> callback)
	{
		m_Label = propertyLabel;
		m_Data = initialValue;
		onValueModified = callback;
	}

	void ColorDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::ColorEdit3(m_Label.data(), glm::value_ptr(m_Data)))
			{
				if (onValueModified)
				{
					m_Modified = true;
					onValueModified(m_Data);
					m_Modified = false;
				}
			}
			ImGui::EndTable();
		}
	}
}