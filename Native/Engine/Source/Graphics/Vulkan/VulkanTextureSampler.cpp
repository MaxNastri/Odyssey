#include "VulkanTextureSampler.h"
#include "VulkanContext.h"
#include "volk.h"
#include <Logger.h>

namespace Odyssey
{
	VulkanTextureSampler::VulkanTextureSampler(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Context->GetDeviceVK(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			Logger::LogError("(VulkanTextureSampler) Failed to create texture sampler.");
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