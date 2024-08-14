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
		VulkanVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv0);
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array< VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();

	public:
		glm::vec3 m_Position;
		glm::vec3 m_Normal;
		glm::vec2 m_UV0;
		glm::vec3 m_Color;
	};
}