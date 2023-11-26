#pragma once
#include "Drawcall.h"
#include "VulkanFrame.h"

namespace Odyssey
{
	struct PerFrameRenderingData
	{
	public:
		std::vector<Drawcall> m_Drawcalls;
		VulkanFrame* frame;
		uint32_t width, height;
		ResourceHandle<VulkanDescriptorBuffer> descriptorBuffer;
		ResourceHandle<VulkanBuffer> uniformBuffer;
	};
}