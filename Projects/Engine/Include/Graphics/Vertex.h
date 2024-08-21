#pragma once
#include "VulkanGlobals.h"
#include <glm.h>

namespace Odyssey
{
	struct Vertex
	{
	public:
		Vertex() = default;
		Vertex(glm::vec3 position, glm::vec3 color);
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv0);

	public:
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array< VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();

	public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord0;
		glm::vec3 Color;
	};
}