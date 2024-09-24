#pragma once
#include "PropertyDrawer.h"
#include <glm.h>

namespace Odyssey
{
	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(std::string_view propertyLabel, glm::vec3 initialValue, glm::vec3 resetValue, bool drawButtons, std::function<void(glm::vec3)> callback);

	public:
		virtual void Draw() override;
		void SetValue(glm::vec3 value) { data = { value.x, value.y, value.z }; }

	private:
		std::function<void(glm::vec3)> onValueModified;
		std::array<float, 3> data{ 0.0f };
		glm::vec3 m_ResetValue;
		bool m_DrawButtons = false;
	};
}