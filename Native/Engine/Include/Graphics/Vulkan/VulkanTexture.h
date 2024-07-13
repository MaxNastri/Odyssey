#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"
#include "ResourceHandle.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class VulkanTextureSampler;

	class VulkanTexture : public Resource
	{
	public:
		VulkanTexture() = default;
		VulkanTexture(VulkanImageDescription description, const void* pixelData);

	public:
		ResourceHandle<VulkanImage> GetImage() { return m_Image; }
		ResourceHandle<VulkanTextureSampler> GetSampler() { return m_Sampler; }
		VkWriteDescriptorSet GetDescriptorInfo();

	public:
		void SetSampler(ResourceHandle<VulkanTextureSampler> sampler) { m_Sampler = sampler; }
		
	private:
		ResourceHandle<VulkanImage> m_Image;
		ResourceHandle<VulkanTextureSampler> m_Sampler;
		VkDescriptorImageInfo descriptor;
	};
}