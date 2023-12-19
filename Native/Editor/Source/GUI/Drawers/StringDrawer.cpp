#include "StringDrawer.h"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace Odyssey
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
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputText("##string", &data))
		{
			m_Modified = true;
			valueUpdatedCallback(data);
		}
	}

	ReadOnlyStringDrawer::ReadOnlyStringDrawer(const std::string& propertyLabel, std::string initialValue)
	{
		m_Label = propertyLabel;
		data = initialValue;
	}

	void ReadOnlyStringDrawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		ImGui::TextUnformatted(data.data());
	}
}