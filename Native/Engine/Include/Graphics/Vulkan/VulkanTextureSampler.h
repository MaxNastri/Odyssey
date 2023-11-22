#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkSampler)

namespace Odyssey
{
	class VulkanContext;

	class VulkanTextureSampler
	{
	public:
		VulkanTextureSampler(std::shared_ptr<VulkanContext> context);
		void Destroy();

	public:
		VkSampler GetSamplerVK() { return m_TextureSampler; }
	private:
		float GetMaxSupportedAniso();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkSampler m_TextureSampler;
	};
}