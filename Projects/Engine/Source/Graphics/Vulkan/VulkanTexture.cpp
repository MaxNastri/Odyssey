#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	VulkanTexture::VulkanTexture(ResourceID id)
		: Resource(id)
	{
	}

	VulkanTexture::VulkanTexture(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription description, BinaryBuffer& buffer)
		: Resource(id)
	{
		m_Image = ResourceManager::Allocate<VulkanImage>(description);
		auto image = ResourceManager::GetResource<VulkanImage>(m_Image);
		image->SetData(buffer, description.ArrayDepth);

		m_Sampler = ResourceManager::Allocate<VulkanTextureSampler>(m_Image);
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