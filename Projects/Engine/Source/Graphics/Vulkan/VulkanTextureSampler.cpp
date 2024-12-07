#include "VulkanTextureSampler.h"
#include "VulkanContext.h"
#include "volk.h"
#include <Log.h>

namespace Odyssey
{
	VulkanTextureSampler::VulkanTextureSampler(ResourceID id, std::shared_ptr<VulkanContext> context)
		: Resource(id)
	{
		m_Context = context;

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = samplerInfo.addressModeU;
		samplerInfo.addressModeW = samplerInfo.addressModeU;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;

		if (vkCreateSampler(m_Context->GetDeviceVK(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			Log::Error("(VulkanTextureSampler) Failed to create texture sampler.");
			return;
		}
	}

	void VulkanTextureSampler::Destroy()
	{
		vkDestroySampler(m_Context->GetDeviceVK(), m_TextureSampler, nullptr);
	}

	float VulkanTextureSampler::GetMaxSupportedAniso()
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_Context->GetPhysicalDeviceVK(), &properties);
		return properties.limits.maxSamplerAnisotropy;
	}
}