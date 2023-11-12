#pragma once
#include "PropertyDrawer.h"
#include <Vector3.h>

namespace Odyssey::Editor
{
	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(const std::string& propertyLabel, Vector3 vec3, std::function<void(Vector3)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(Vector3)> onValueModified;
		std::array<float, 3> data{ 0.0f };
	};
}