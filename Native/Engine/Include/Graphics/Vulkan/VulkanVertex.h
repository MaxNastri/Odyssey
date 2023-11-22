#pragma once
#include "VulkanGlobals.h"
#include <glm.h>

namespace Odyssey
{
	class VulkanVertex
	{
	public:
		VulkanVertex() = default;
		VulkanVertex(glm::vec3 position, glm::vec3 color);
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array< VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Color;
	};
}