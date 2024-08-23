#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanContext> context, VulkanImageDescription description, BinaryBuffer& buffer)
	{
		m_Image = ResourceManager::Allocate<VulkanImage>(description);
		auto image = ResourceManager::GetResource<VulkanImage>(m_Image);
		image->SetData(buffer);

		m_Sampler = ResourceManager::Allocate<VulkanTextureSampler>();
		auto sampler = ResourceManager::GetResource<VulkanTextureSampler>(m_Sampler);

		descriptor.imageView = image->GetImageView();
		descriptor.imageLayout = image->GetLayout();
		descriptor.sampler = sampler->GetSamplerVK();
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