#include "VulkanRenderTexture.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "ResourceManager.h"
#include <Logger.h>
#include "volk.h"
#include "VulkanImage.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

namespace Odyssey
{
	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;

		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.ImageType = TextureType::RenderTexture;
		imageDesc.Format = TextureFormat::R8G8B8A8_UNORM;

		m_Image = ResourceManager::Allocate<VulkanImage>(imageDesc);

		// Allocate a command buffer to transition the image layout
		auto commandPoolID = context->GetCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		auto commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Transition the image to the proper layout
		commandBuffer->BeginCommands();
		commandBuffer->TransitionLayouts(m_Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		commandBuffer->EndCommands();
		commandBuffer->Flush();

		// Release the command buffer
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, uint32_t width, uint32_t height, TextureFormat format)
	{
		m_Context = context;
		m_Width = width;
		m_Height = height;

		// Allocate the image
		VulkanImageDescription imageDesc;
		imageDesc.Width = width;
		imageDesc.Height = height;
		imageDesc.Format = format;
		imageDesc.ImageType = IsDepthTexture(format) ? TextureType::DepthTexture : TextureType::RenderTexture;
		m_Image = ResourceManager::Allocate<VulkanImage>(imageDesc);

		// Allocate a command buffer to transition the image layout
		auto commandPoolID = context->GetCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		auto commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Transition the image to the proper layout based on format
		VkImageLayout layout = IsDepthTexture(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		commandBuffer->BeginCommands();
		commandBuffer->TransitionLayouts(m_Image, layout);
		commandBuffer->EndCommands();
		commandBuffer->Flush();

		// Release the command buffer
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VulkanRenderTexture::VulkanRenderTexture(std::shared_ptr<VulkanContext> context, ResourceID imageID, TextureFormat format)
	{
		m_Context = context;

		if (auto image = ResourceManager::GetResource<VulkanImage>(imageID))
		{
			m_Width = image->GetWidth();
			m_Height = image->GetHeight();
			m_Image = imageID;
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

	bool VulkanRenderTexture::IsDepthTexture(TextureFormat format)
	{
		return format == TextureFormat::D32_SFLOAT || format == TextureFormat::D32_SFLOAT_S8_UINT ||
			format == TextureFormat::D24_UNORM_S8_UINT;
	}
}