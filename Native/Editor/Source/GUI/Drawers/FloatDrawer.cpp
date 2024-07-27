#include "FloatDrawer.h"
#include <imgui.h>

namespace Odyssey
{
	FloatDrawer::FloatDrawer(const std::string& propertyLabel, float initialValue, std::function<void(float)> callback)
		: PropertyDrawer(propertyLabel)
	{
		data = initialValue;
		valueUpdatedCallback = callback;
	}

	void FloatDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::InputFloat(m_Label.data(), &data, step, stepFast))
			{
				valueUpdatedCallback(data);
			}
			ImGui::EndTable();
		}
	}
}