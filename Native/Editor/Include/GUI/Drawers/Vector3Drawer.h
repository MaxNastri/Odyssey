#pragma once
#include "PropertyDrawer.h"
#include <glm.h>

namespace Odyssey::Editor
{
	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(const std::string& propertyLabel, glm::vec3 vec3, std::function<void(glm::vec3)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(glm::vec3)> onValueModified;
		std::array<float, 3> data{ 0.0f };
	};
}