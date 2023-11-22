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
	};

	enum class ImageType
	{
		None = 0,
		Image2D = 1,
		RenderTexture = 2,
	};

	enum class ImageFormat
	{
		None = 0,
		R8G8B8A8_SRGB = 1
	};

	enum class ImageTiling
	{
		None = 0,
		Optimal = 1,
		Linear = 2,

	};

	enum class ShaderType
	{
		None = 0,
		Fragment = 1,
		Vertex = 2,
	};
}
