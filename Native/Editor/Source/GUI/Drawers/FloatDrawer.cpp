#include "FloatDrawer.h"
#include <imgui.h>

namespace Odyssey::Editor
{
	FloatDrawer::FloatDrawer(const std::string& propertyLabel, float initialValue, std::function<void(float)> callback)
		: PropertyDrawer(propertyLabel)
	{
		value = initialValue;
		valueUpdatedCallback = callback;
	}

	void FloatDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputFloat(label.data(), &value, step, stepFast))
		{
			valueUpdatedCallback(value);
		}
	}
}