#include "VulkanRenderTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "ResourceManager.h"
#include <Log.h>
#include "volk.h"
#include "VulkanImage.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

namespace Odyssey
{
	VulkanRenderTexture::VulkanRenderTexture(ResourceID id, std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height)
		: Resource(id)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;
		m_Format = TextureFormat::R8G8B8A8_UNORM;

		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.ImageType = ImageType::RenderTexture;
		imageDesc.Format = TextureFormat::R8G8B8A8_UNORM;
		imageDesc.Samples = context->GetSampleCount();

		m_Image = ResourceManager::Allocate<VulkanImage>(imageDesc);

		if (imageDesc.Samples > 1)
		{
			VulkanImageDescription resolveDesc = imageDesc;
			resolveDesc.Samples = 1;
			m_ResolveImage = ResourceManager::Allocate<VulkanImage>(resolveDesc);
		}

		// Allocate a command buffer to transition the image layout
		auto commandPoolID = context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		auto commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Transition the image to the proper layout
		commandBuffer->BeginCommands();
		commandBuffer->TransitionLayouts(m_Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		if (m_ResolveImage.IsValid())
			commandBuffer->TransitionLayouts(m_ResolveImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();

		// Release the command buffer
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VulkanRenderTexture::VulkanRenderTexture(ResourceID id, std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height, TextureFormat format)
		: Resource(id)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;
		m_Format = format;

		// Allocate the image
		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.Format = format;
		imageDesc.ImageType = IsDepthTexture(format) ? ImageType::DepthTexture : ImageType::RenderTexture;
		imageDesc.Samples = m_Context->GetSampleCount();

		m_Image = ResourceManager::Allocate<VulkanImage>(imageDesc);

		if (imageDesc.Samples > 1)
		{
			VulkanImageDescription resolveDesc = imageDesc;
			resolveDesc.Samples = 1;
			m_ResolveImage = ResourceManager::Allocate<VulkanImage>(resolveDesc);
		}

		// Allocate a command buffer to transition the image layout
		auto commandPoolID = context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		auto commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Transition the image to the proper layout based on format
		VkImageLayout layout = IsDepthTexture(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		commandBuffer->BeginCommands();
		commandBuffer->TransitionLayouts(m_Image, layout);
		if (m_ResolveImage.IsValid())
			commandBuffer->TransitionLayouts(m_ResolveImage, layout);
		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();

		// Release the command buffer
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VulkanRenderTexture::VulkanRenderTexture(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& imageDesc)
		: Resource(id)
	{
		m_Context = context;
		m_Width = imageDesc.Width;
		m_Height = imageDesc.Height;
		m_Format = imageDesc.Format;

		// Allocate the image
		m_Image = ResourceManager::Allocate<VulkanImage>(imageDesc);

		if (imageDesc.Samples > 1)
		{
			VulkanImageDescription resolveDesc = imageDesc;
			resolveDesc.Samples = 1;
			m_ResolveImage = ResourceManager::Allocate<VulkanImage>(resolveDesc);
		}

		if (imageDesc.ImageType == ImageType::Shadowmap)
		{
			m_Sampler = ResourceManager::Allocate<VulkanTextureSampler>();
		}

		// Allocate a command buffer to transition the image layout
		auto commandPoolID = context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		auto commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Transition the image to the proper layout based on format
		VkImageLayout layout = IsDepthTexture(imageDesc.Format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		commandBuffer->BeginCommands();
		commandBuffer->TransitionLayouts(m_Image, layout);
		if (m_ResolveImage.IsValid())
			commandBuffer->TransitionLayouts(m_ResolveImage, layout);
		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();

		// Release the command buffer
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VulkanRenderTexture::VulkanRenderTexture(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID imageID, TextureFormat format)
		: Resource(id)
	{
		m_Context = context;

		if (Ref<VulkanImage> image = ResourceManager::GetResource<VulkanImage>(imageID))
		{
			m_Width = image->GetWidth();
			m_Height = image->GetHeight();
			m_Format = format;

			if (m_Context->GetSampleCount() > 1)
			{
				// Store the original image as the single-sample resolve
				m_ResolveImage = imageID;

				// Create a new msaa image
				VulkanImageDescription msaaDesc;
				msaaDesc.Width = m_Width;
				msaaDesc.Height = m_Height;
				msaaDesc.ImageType = ImageType::RenderTexture;
				msaaDesc.Samples = m_Context->GetSampleCount();
				msaaDesc.Format = m_Format;
				m_Image = ResourceManager::Allocate<VulkanImage>(msaaDesc);
			}
			else
			{
				m_Image = imageID;
			}
		}
	}

	void VulkanRenderTexture::Destroy()
	{
		ResourceManager::Destroy(m_Image);
	}

	void VulkanRenderTexture::SetData(BinaryBuffer& buffer)
	{
		if (m_Image)
		{
			auto image = ResourceManager::GetResource<VulkanImage>(m_Image);
			image->SetData(buffer);
		}
	}

	VkWriteDescriptorSet VulkanRenderTexture::GetDescriptorInfo()
	{
		auto sampler = ResourceManager::GetResource<VulkanTextureSampler>(m_Sampler);
		auto image = ResourceManager::GetResource<VulkanImage>(m_Image);

		descriptor.imageView = image->GetImageView();
		descriptor.imageLayout = image->GetLayout();
		descriptor.sampler = sampler->GetSamplerVK();

		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = 0;
		writeSet.dstBinding = 2;
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeSet.pImageInfo = &descriptor;
		return writeSet;
	}

	bool VulkanRenderTexture::IsDepthTexture(TextureFormat format)
	{
		return format == TextureFormat::D32_SFLOAT || format == TextureFormat::D32_SFLOAT_S8_UINT ||
			format == TextureFormat::D24_UNORM_S8_UINT || format == TextureFormat::D16_UNORM;
	}
}