#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"

VK_FWD_DECLARE(VkSampler)

namespace Odyssey
{
	class VulkanContext;

	class VulkanTextureSampler : public Resource
	{
	public:
		VulkanTextureSampler(ResourceID id, std::shared_ptr<VulkanContext> context);
		VulkanTextureSampler(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID imageID, bool clamp = false);
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