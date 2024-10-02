#include "ColorDrawer.h"
#include "imgui.h"

namespace Odyssey
{
	Color3Drawer::Color3Drawer(std::string_view propertyLabel, float3 initialValue, std::function<void(float3)> callback)
	{
		m_Label = propertyLabel;
		m_Data = initialValue;
		onValueModified = callback;
	}

	void Color3Drawer::Draw()
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

	Color4Drawer::Color4Drawer(std::string_view propertyLabel, float4 initialValue, std::function<void(float4)> callback)
	{
		m_Label = propertyLabel;
		m_Data = initialValue;
		onValueModified = callback;
	}

	void Color4Drawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::ColorEdit4(m_Label.data(), glm::value_ptr(m_Data)))
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