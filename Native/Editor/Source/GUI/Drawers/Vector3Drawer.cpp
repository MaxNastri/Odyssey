#include "Vector3Drawer.h"
#include <imgui.h>

namespace Odyssey
{
	Vector3Drawer::Vector3Drawer(const std::string& propertyLabel, glm::vec3 vec3, std::function<void(glm::vec3)> callback)
	{
		m_Label = propertyLabel;
		data = { vec3.x, vec3.y, vec3.z };
		onValueModified = callback;
	}

	void Vector3Drawer::Draw()
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(m_Label.data());
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-0.01f);
		if (ImGui::InputFloat3(m_Label.data(), data.data()))
		{
			onValueModified(glm::vec3(data[0], data[1], data[2]));
		}
	}
}