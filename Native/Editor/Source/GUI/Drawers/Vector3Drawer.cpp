#include "Vector3Drawer.h"
#include <imgui.h>
#include "imgui_internal.h"

namespace Odyssey
{
	Vector3Drawer::Vector3Drawer(const std::string& propertyLabel, glm::vec3 initialValue, glm::vec3 resetValue, std::function<void(glm::vec3)> callback)
	{
		m_Label = propertyLabel;
		data = { initialValue.x, initialValue.y, initialValue.z };
		m_ResetValue = resetValue;
		onValueModified = callback;
	}

	void Vector3Drawer::Draw()
	{
		ImGui::PushID((void*)this);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(m_Label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 5.0f, lineHeight };

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			if (ImGui::Button("X", buttonSize))
			{
				data[0] = m_ResetValue.x;
				m_Modified = true;
			}
			ImGui::SameLine();
			m_Modified |= ImGui::DragFloat("##x", &data[0], 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PopStyleColor(3);
		}

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			if (ImGui::Button("Y", buttonSize))
			{
				data[1] = m_ResetValue.y;
				m_Modified = true;
			}
			ImGui::SameLine();
			m_Modified |= ImGui::DragFloat("##y", &data[1], 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PopStyleColor(3);
		}

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			if (ImGui::Button("Z", buttonSize))
			{
				data[2] = m_ResetValue.z;
				m_Modified = true;
			}
			ImGui::SameLine();
			m_Modified |= ImGui::DragFloat("##z", &data[2], 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::PopStyleColor(3);

		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		if (m_Modified)
		{
			onValueModified(glm::vec3(data[0], data[1], data[2]));
			m_Modified = false;
		}
	}
}