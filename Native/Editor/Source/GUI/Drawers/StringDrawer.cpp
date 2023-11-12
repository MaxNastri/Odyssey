#include "StringDrawer.h"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace Odyssey::Editor
{
	StringDrawer::StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(std::string)> callback)
		: PropertyDrawer(propertyLabel)
	{
		data = initialValue;
		valueUpdatedCallback = callback;
	}

	void StringDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputText("##string", &data))
		{
			valueUpdatedCallback(data);
		}
	}
}