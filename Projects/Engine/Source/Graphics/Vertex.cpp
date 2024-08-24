#include "Vertex.h"

namespace Odyssey
{
	Vertex::Vertex(glm::vec3 position, glm::vec4 color)
	{
		Position = position;
		Color = color;
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv0)
	{
		Position = position;
		Normal = normal;
		TexCoord0 = uv0;
	}

	VkVertexInputBindingDescription Vertex::GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 12> Vertex::GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 12> attributeDescriptions;

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Tangent);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, Color);

		attributeDescriptions[4].binding = 0;
		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(Vertex, TexCoord0);

		attributeDescriptions[5].binding = 0;
		attributeDescriptions[5].location = 5;
		attributeDescriptions[5].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[5].offset = offsetof(Vertex, TexCoord1);

		attributeDescriptions[6].binding = 0;
		attributeDescriptions[6].location = 6;
		attributeDescriptions[6].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[6].offset = offsetof(Vertex, TexCoord2);

		attributeDescriptions[7].binding = 0;
		attributeDescriptions[7].location = 7;
		attributeDescriptions[7].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[7].offset = offsetof(Vertex, TexCoord3);

		attributeDescriptions[8].binding = 0;
		attributeDescriptions[8].location = 8;
		attributeDescriptions[8].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[8].offset = offsetof(Vertex, TexCoord4);

		attributeDescriptions[9].binding = 0;
		attributeDescriptions[9].location = 9;
		attributeDescriptions[9].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[9].offset = offsetof(Vertex, TexCoord5);

		attributeDescriptions[10].binding = 0;
		attributeDescriptions[10].location = 10;
		attributeDescriptions[10].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[10].offset = offsetof(Vertex, TexCoord6);

		attributeDescriptions[11].binding = 0;
		attributeDescriptions[11].location = 11;
		attributeDescriptions[11].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[11].offset = offsetof(Vertex, TexCoord7);

		//attributeDescriptions[12].binding = 0;
		//attributeDescriptions[12].location = 12;
		//attributeDescriptions[12].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//attributeDescriptions[12].offset = offsetof(Vertex, BoneWeights);
		//
		//attributeDescriptions[13].binding = 0;
		//attributeDescriptions[13].location = 13;
		//attributeDescriptions[13].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//attributeDescriptions[13].offset = offsetof(Vertex, BoneIndices);

		return attributeDescriptions;
	}
}