#include "FloatDrawer.h"
#include <imgui.h>

namespace Odyssey::Editor
{
	FloatDrawer::FloatDrawer(const std::string& propertyLabel, float initialValue)
		: PropertyDrawer(propertyLabel)
	{
		value = initialValue;
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
	void FloatDrawer::SetCallback(std::function<void(float)> callback)
	{
		valueUpdatedCallback = callback;
	}
}