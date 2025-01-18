#pragma once
#include "Resource.h"
#include "SpriteRenderer.h"

namespace Odyssey
{
	struct Drawcall
	{
	public:
		Drawcall() = default;
		Drawcall(ResourceID vertexBufferID, ResourceID indexBufferID, uint32_t indexCount);

	public:
		std::bitset<16> RenderLayers;
		ResourceID VertexBufferID;
		ResourceID IndexBufferID;
		uint32_t IndexCount;
		uint32_t UniformBufferIndex;
		bool Skinned = false;
		bool SkipDepth = false;
	};

	struct SpriteDrawcall
	{
	public:
		SpriteDrawcall() = default;

	public:
		SpriteRenderer::AnchorPosition Anchor;
		float2 Position;
		float2 Scale;
		float2 Fill;
		float4 BaseColor;
		ResourceID Sprite;
	};
}