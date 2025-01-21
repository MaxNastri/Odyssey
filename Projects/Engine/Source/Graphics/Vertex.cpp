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

	uint32_t Vertex::GetOffset(std::string_view attribute)
	{
		if (attribute == "position")
			return offsetof(Vertex, Position);
		else if (attribute == "normal")
			return offsetof(Vertex, Normal);
		else if (attribute == "tangent")
			return offsetof(Vertex, Tangent);
		else if (attribute == "color")
			return offsetof(Vertex, Color);
		else if (attribute == "texcoord0")
			return offsetof(Vertex, TexCoord0);
		else if (attribute == "texcoord1")
			return offsetof(Vertex, TexCoord1);
		else if (attribute == "texcoord2")
			return offsetof(Vertex, TexCoord2);
		else if (attribute == "texcoord3")
			return offsetof(Vertex, TexCoord3);
		else if (attribute == "texcoord4")
			return offsetof(Vertex, TexCoord4);
		else if (attribute == "texcoord5")
			return offsetof(Vertex, TexCoord5);
		else if (attribute == "texcoord6")
			return offsetof(Vertex, TexCoord6);
		else if (attribute == "texcoord7")
			return offsetof(Vertex, TexCoord7);
		else if (attribute == "boneindices")
			return offsetof(Vertex, BoneIndices);
		else if (attribute == "boneweights")
			return offsetof(Vertex, BoneWeights);

		return 0;
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