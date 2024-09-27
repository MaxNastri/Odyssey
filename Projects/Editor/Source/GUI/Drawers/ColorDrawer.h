#pragma once
#include "PropertyDrawer.h"
namespace Odyssey
{
	class ColorDrawer : public PropertyDrawer
	{
	public:
		ColorDrawer() = default;
		ColorDrawer(std::string_view propertyLabel, glm::vec3 initialValue, std::function<void(glm::vec3)> callback);

	public:
		virtual void Draw() override;
		void SetValue(glm::vec3 value) { m_Data = value; }

	private:
		std::function<void(glm::vec3)> onValueModified;
		glm::vec3 m_Data;
	};
}