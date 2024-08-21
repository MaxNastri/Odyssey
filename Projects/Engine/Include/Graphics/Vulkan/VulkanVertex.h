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
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord0;
		glm::vec3 Color;
	};
}