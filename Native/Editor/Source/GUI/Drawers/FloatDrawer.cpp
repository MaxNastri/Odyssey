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
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputFloat(m_Label.data(), &data, step, stepFast))
		{
			valueUpdatedCallback(data);
		}
	}
}