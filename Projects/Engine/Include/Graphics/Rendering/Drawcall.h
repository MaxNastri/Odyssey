#pragma once
#include "Resource.h"

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
	};

	struct SpriteDrawcall
	{
	public:
		SpriteDrawcall() = default;
		SpriteDrawcall(float2 position, float2 scale, ResourceID sprite)
		{
			Position = position;
			Scale = scale;
			Sprite = sprite;
		}

	public:
		float2 Position;
		float2 Scale;
		ResourceID Sprite;
	};
}