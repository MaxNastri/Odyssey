#pragma once
#include "VulkanGlobals.h"
#include <glm.h>

namespace Odyssey
{
	struct alignas(4) Vertex
	{
	public:
		Vertex();
		Vertex(glm::vec3 position, glm::vec4 color);
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv0);

	public:
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec4 Color;
		glm::vec2 TexCoord0;
		glm::vec2 TexCoord1;
		glm::vec2 TexCoord2;
		glm::vec2 TexCoord3;
		glm::vec2 TexCoord4;
		glm::vec2 TexCoord5;
		glm::vec2 TexCoord6;
		glm::vec2 TexCoord7;
		glm::vec4 BoneIndices;
		glm::vec4 BoneWeights;
	};
}