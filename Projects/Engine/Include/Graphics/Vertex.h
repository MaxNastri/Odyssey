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
		static uint32_t GetOffset(std::string_view attribute);
		static VkVertexInputBindingDescription GetBindingDescription();

	public:
		float3 Position = float3(0, 0, 0);
		float3 Normal = float3(0, 1, 0);
		float4 Tangent = float4(0, 1, 0, 0);
		float4 Color = float4(1, 1, 1, 1);
		float2 TexCoord0 = float2(0, 0);
		float2 TexCoord1 = float2(0, 0);
		float2 TexCoord2 = float2(0, 0);
		float2 TexCoord3 = float2(0, 0);
		float2 TexCoord4 = float2(0, 0);
		float2 TexCoord5 = float2(0, 0);
		float2 TexCoord6 = float2(0, 0);
		float2 TexCoord7 = float2(0, 0);
		float4 BoneIndices = float4(0, 0, 0, 0);
		float4 BoneWeights = float4(0, 0, 0, 0);
	};
}