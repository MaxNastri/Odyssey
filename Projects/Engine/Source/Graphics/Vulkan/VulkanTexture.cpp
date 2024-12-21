#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace Odyssey
{
	VulkanTexture::VulkanTexture(ResourceID id)
		: Resource(id)
	{
	}

	bool IsRenderTexture(VulkanImageDescription& desc)
	{
		return desc.ImageType == ImageType::RenderTexture || desc.ImageType == ImageType::DepthTexture || desc.ImageType == ImageType::Shadowmap;
	}

	VulkanTexture::VulkanTexture(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription description, BinaryBuffer* buffer)
		: Resource(id)
	{
		m_Description = description;

		// Create the image and set the initial pixel data
		m_Image = ResourceManager::Allocate<VulkanImage>(description);
		auto image = ResourceManager::GetResource<VulkanImage>(m_Image);

		if (buffer)
			image->SetData(*buffer, description.ArrayDepth);

		// Create a sampler
		m_Sampler = ResourceManager::Allocate<VulkanTextureSampler>(m_Image);
		auto sampler = ResourceManager::GetResource<VulkanTextureSampler>(m_Sampler);

		// Render textures need to transition to an appropriate layout on creation
		if (IsRenderTexture(m_Description))
		{
			// Allocate a command buffer to transition the image layout
			auto commandPoolID = context->GetGraphicsCommandPool();
			auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
			auto commandBufferID = commandPool->AllocateBuffer();
			auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

			// Transition the image to the proper layout based on format
			VkImageLayout layout = m_Description.ImageType == ImageType::RenderTexture ?
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			commandBuffer->BeginCommands();
			commandBuffer->TransitionLayouts(m_Image, layout);
			commandBuffer->EndCommands();
			commandBuffer->SubmitGraphics();
		}

		// Fill in the descriptor write set for binding to the shader
		descriptor.imageView = image->GetImageView();
		descriptor.imageLayout = image->GetLayout();
		descriptor.sampler = sampler->GetSamplerVK();
	}

	VulkanTexture::VulkanTexture(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID imageID, TextureFormat format)
	{
		if (Ref<VulkanImage> image = ResourceManager::GetResource<VulkanImage>(imageID))
		{
			m_Description.Width = image->GetWidth();
			m_Description.Height = image->GetHeight();
			m_Description.Format = format;
			m_Image = imageID;
		}
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