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
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputDouble(label.data(), &value, step, stepFast))
		{
			valueUpdatedCallback(value);
		}
	}
}