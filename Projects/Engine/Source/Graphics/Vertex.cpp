#include "Vertex.h"

namespace Odyssey
{
	Vertex::Vertex()
	{
		Position = glm::vec3(0, 0, 0);
		Normal = glm::vec3(0, 0, 0);
		Tangent = float4(0.0f);
		Color = glm::vec4(0, 0, 0, 0);
		TexCoord0 = glm::vec2(0, 0);
		TexCoord1 = glm::vec2(0, 0);
		TexCoord2 = glm::vec2(0, 0);
		TexCoord3 = glm::vec2(0, 0);
		TexCoord4 = glm::vec2(0, 0);
		TexCoord5 = glm::vec2(0, 0);
		TexCoord6 = glm::vec2(0, 0);
		TexCoord7 = glm::vec2(0, 0);
		BoneIndices = glm::vec4(0, 0, 0, 0);
		BoneWeights = glm::vec4(1, 0, 0, 0);
	}

	Vertex::Vertex(glm::vec3 position, glm::vec4 color)
	{
		Position = position;
		Normal = glm::vec3(0, 0, 0);
		Tangent = float4(0.0f);
		Color = color;
		TexCoord0 = glm::vec2(0, 0);
		TexCoord1 = glm::vec2(0, 0);
		TexCoord2 = glm::vec2(0, 0);
		TexCoord3 = glm::vec2(0, 0);
		TexCoord4 = glm::vec2(0, 0);
		TexCoord5 = glm::vec2(0, 0);
		TexCoord6 = glm::vec2(0, 0);
		TexCoord7 = glm::vec2(0, 0);
		BoneIndices = glm::vec4(0, 0, 0, 0);
		BoneWeights = glm::vec4(1, 0, 0, 0);
	}

	Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv0)
	{
		Position = position;
		Normal = normal;
		Tangent = float4(0.0f);
		Color = glm::vec4(0, 0, 0, 0);
		TexCoord0 = uv0;
		TexCoord1 = glm::vec2(0, 0);
		TexCoord2 = glm::vec2(0, 0);
		TexCoord3 = glm::vec2(0, 0);
		TexCoord4 = glm::vec2(0, 0);
		TexCoord5 = glm::vec2(0, 0);
		TexCoord6 = glm::vec2(0, 0);
		TexCoord7 = glm::vec2(0, 0);
		BoneIndices = glm::vec4(0, 0, 0, 0);
		BoneWeights = glm::vec4(1, 0, 0, 0);
	}

	VkVertexInputBindingDescription Vertex::GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		VkVertexInputAttributeDescription description{};

		// Position
		description.binding = 0;
		description.location = 0;
		description.format = VK_FORMAT_R32G32B32_SFLOAT;
		description.offset = offsetof(Vertex, Position);
		attributeDescriptions.push_back(description);

		// Normal
		description.binding = 0;
		description.location = 1;
		description.format = VK_FORMAT_R32G32B32_SFLOAT;
		description.offset = offsetof(Vertex, Normal);
		attributeDescriptions.push_back(description);

		// Tangent
		description.binding = 0;
		description.location = 2;
		description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		description.offset = offsetof(Vertex, Tangent);
		attributeDescriptions.push_back(description);

		// Color
		description.binding = 0;
		description.location = 3;
		description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		description.offset = offsetof(Vertex, Color);
		attributeDescriptions.push_back(description);

		// TexCoord0
		description.binding = 0;
		description.location = 4;
		description.format = VK_FORMAT_R32G32_SFLOAT;
		description.offset = offsetof(Vertex, TexCoord0);
		attributeDescriptions.push_back(description);

		// Bone Indices
		description.binding = 0;
		description.location = 5;
		description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		description.offset = offsetof(Vertex, BoneIndices);
		attributeDescriptions.push_back(description);

		// Bone Weights
		description.binding = 0;
		description.location = 6;
		description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		description.offset = offsetof(Vertex, BoneWeights);
		attributeDescriptions.push_back(description);

		return attributeDescriptions;
	}
}