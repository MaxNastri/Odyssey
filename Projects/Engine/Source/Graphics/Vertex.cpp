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
}