#pragma once
#include "PropertyDrawer.h"
#include <glm.h>

namespace Odyssey
{
	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(const std::string& propertyLabel, glm::vec3 initialValue, glm::vec3 resetValue, std::function<void(glm::vec3)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(glm::vec3)> onValueModified;
		std::array<float, 3> data{ 0.0f };
		glm::vec3 m_ResetValue;
	};
}