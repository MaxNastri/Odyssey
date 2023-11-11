#include "IntDrawer.h"
#include <Imgui.h>

namespace Odyssey::Editor
{
	IntDrawer::IntDrawer(const std::string& propertyLabel, uint32_t initialValue)
		: PropertyDrawer(propertyLabel)
	{
		data = initialValue;	
	}

	void IntDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputScalar(label.data(), ImGuiDataType_::ImGuiDataType_U32, &data))
		{
			valueUpdatedCallback(data);
		}
	}

	void IntDrawer::SetCallback(std::function<void(uint32_t)> callback)
	{
		valueUpdatedCallback = callback;
	}
}