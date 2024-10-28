#pragma once

namespace Odyssey
{
	enum class BufferType
	{
		None = 0,
		Staging = 1,
		Vertex = 2,
		Index = 3,
		Uniform = 4,
		Storage = 5,
	};

	enum DescriptorType
	{
		None = 0,
		Uniform = 1,
		Sampler = 2,
		Storage = 3,
	};

	enum class ShaderStage
	{
		None = 0,
		Fragment = 1,
		Vertex = 2,
		Compute = 3,
		Geometry = 4,
	};

	enum class TextureType
	{
		None = 0,
		Texture2D = 1,
		Texture3D = 2,
		Cubemap = 3,
	};

	enum class CubemapFace
	{
		None = 0,
		XNeg = 1,
		XPos = 2,
		YNeg = 3,
		YPos = 4,
		ZNeg = 5,
		ZPos = 6,
	};

	enum class ImageType
	{
		None = 0,
		Image2D = 1,
		Image2DArray = 2,
		Cubemap = 3,
		RenderTexture = 4,
		DepthTexture = 5,
	};

	enum class TextureFormat
	{
		None = 0,
		R8G8B8A8_SRGB = 1,
		R8G8B8A8_UNORM = 2,
		R8G8B8_UNORM = 3,
		D32_SFLOAT = 100,
		D32_SFLOAT_S8_UINT = 101,
		D24_UNORM_S8_UINT = 102,
	};

	enum class ImageTiling
	{
		None = 0,
		Optimal = 1,
		Linear = 2,

	};

	enum class ShaderType : uint32_t
	{
		None = 0,
		Fragment = 1,
		Vertex = 2,
		Compute = 3,
		Geometry = 4,
	};
}
