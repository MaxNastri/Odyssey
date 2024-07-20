#include "StringDrawer.h"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace Odyssey
{
	StringDrawer::StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(std::string&)> callback, bool readOnly)
		: PropertyDrawer(propertyLabel)
	{
		data = initialValue;
		valueUpdatedCallback = callback;
		m_ReadOnly = readOnly;
	}

	void StringDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (m_ReadOnly)
				ImGui::BeginDisabled();

			if (ImGui::InputText(m_Label.data(), &data))
			{
				m_Modified = true;
				valueUpdatedCallback(data);
			}
			if (m_ReadOnly)
				ImGui::EndDisabled();
			ImGui::EndTable();
		}
	}

	ReadOnlyStringDrawer::ReadOnlyStringDrawer(const std::string& propertyLabel, std::string initialValue)
	{
		m_Label = propertyLabel;
		m_Data = initialValue;
	}

	void ReadOnlyStringDrawer::Draw()
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			ImGui::TextUnformatted(m_Data.data());
			ImGui::EndTable();
		}
	}
}