#include "DoubleDrawer.h"
#include <imgui.h>

namespace Odyssey
{
	DoubleDrawer::DoubleDrawer(const std::string& propertyLabel, double initialValue, std::function<void(double)> callback)
		: PropertyDrawer(propertyLabel)
	{
		value = initialValue;
		valueUpdatedCallback = callback;
	}

	void DoubleDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::InputDouble(m_Label.data(), &value, step, stepFast))
			{
				valueUpdatedCallback(value);
			}
			ImGui::EndTable();
		}
	}
}