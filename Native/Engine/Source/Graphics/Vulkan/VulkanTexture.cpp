#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	VulkanTexture::VulkanTexture(VulkanImageDescription description, const void* pixelData)
	{
		m_Image = ResourceManager::AllocateImage(description);
		m_Image.Get()->SetData(pixelData);

		m_Sampler = ResourceManager::AllocateSampler();

		descriptor.imageView = m_Image.Get()->GetImageView();
		descriptor.imageLayout = m_Image.Get()->GetLayout();
		descriptor.sampler = m_Sampler.Get()->GetSamplerVK();
	}
	VkWriteDescriptorSet VulkanTexture::GetDescriptorInfo()
	{
		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = 0;
		writeSet.dstBinding = 2;
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeSet.pImageInfo = &descriptor;
		return writeSet;
	}
}