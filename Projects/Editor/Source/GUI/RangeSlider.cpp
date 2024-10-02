#include "RangeSlider.h"
#include "imgui.h"

namespace Odyssey
{
	RangeSlider::RangeSlider(const std::string& label, float2 range, float2 limits, float step, std::function<void(float2)> callback)
		: PropertyDrawer(label)
	{
		m_Range = range;
		m_Limits = limits;
		m_Step = step;
		m_ValueUpdatedCallback = callback;
	}

	void RangeSlider::Draw()
	{
		if (ImGui::BeginTable("#RangeSlider", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::DragFloatRange2(m_Label.data(), &m_Range.x, &m_Range.y, m_Step, m_Limits.x, m_Limits.y,
				"Min: %.2f", "Max: %.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				if (m_ValueUpdatedCallback)
				{
					m_Modified = true;
					m_ValueUpdatedCallback(m_Range);
					m_Modified = false;
				}
			}
			ImGui::EndTable();
		}
	}
}