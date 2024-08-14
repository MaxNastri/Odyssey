#pragma once

namespace Odyssey
{
	class RenderScene;
	class VulkanFrame;

	struct PerFrameRenderingData
	{
	public:
		std::shared_ptr<RenderScene> renderScene;
		VulkanFrame* frame;
		uint32_t width, height;
	};
}