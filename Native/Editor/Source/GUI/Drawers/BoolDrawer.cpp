#include "BoolDrawer.h"
#include <imgui.h>

namespace Odyssey
{
	BoolDrawer::BoolDrawer(const std::string& propertyLabel, bool initialValue, std::function<void(bool)> callback)
		: PropertyDrawer(propertyLabel)
	{
		data = initialValue;
		valueUpdatedCallback = callback;
	}

	void BoolDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::Checkbox("##label", &data))
		{
			valueUpdatedCallback(data);
		}
	}
}