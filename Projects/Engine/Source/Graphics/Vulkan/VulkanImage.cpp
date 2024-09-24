#include "VulkanImage.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandPool.h"
#include "Vulkancommandbuffer.h"
#include <Logger.h>
#include "ResourceManager.h"
#include "VulkanBuffer.h"

namespace Odyssey
{
	VulkanImage::VulkanImage(std::shared_ptr<VulkanContext> context, VulkanImageDescription& desc)
	{
		m_Context = context;
		m_Width = desc.Width;
		m_Height = desc.Height;
		m_Channels = desc.Channels;
		isDepth = desc.ImageType == ImageType::DepthTexture;

		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
		VkPhysicalDevice physicalDevice = m_Context->GetPhysicalDevice()->GetPhysicalDevice();

		// Image creation
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = desc.Width;
			imageInfo.extent.height = desc.Height;
			imageInfo.extent.depth = desc.Depth;
			imageInfo.mipLevels = desc.MipLevels;
			imageInfo.arrayLayers = desc.ArrayLayers;
			imageInfo.format = GetFormat(desc.Format);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = GetUsage(desc.ImageType);
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0; // Optional

			if (vkCreateImage(device, &imageInfo, allocator, &m_Image) != VK_SUCCESS)
			{
				Logger::LogError("[VulkanImage] Failed to create image");
				return;
			}
		}

		// Image memory
		{
			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			{
				Logger::LogError("(VulkanImage) Failed to allocate image memory");
				return;
			}

			vkBindImageMemory(device, m_Image, imageMemory, 0);
		}

		// Image view
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_Image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = GetFormat(desc.Format);
			viewInfo.subresourceRange.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.levelCount = desc.MipLevels;
			viewInfo.subresourceRange.layerCount = desc.ArrayLayers;

			if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				Logger::LogError("(VulkanImage) Failed to create image view.");
				return;
			}
		}
	}

	VulkanImage::VulkanImage(std::shared_ptr<VulkanContext> context, VkImage image, uint32_t width, uint32_t height, uint32_t channels, VkFormat format)
	{
		m_Context = context;
		m_Image = image;
		m_Width = width;
		m_Height = height;
		m_Channels = channels;
		isDepth = false;

		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// Image view
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(context->GetDeviceVK(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				Logger::LogError("(VulkanImage) Failed to create image view.");
				return;
			}
		}
	}

	void VulkanImage::Destroy()
	{
		vkDestroyImage(m_Context->GetDeviceVK(), m_Image, allocator);
		vkDestroyImageView(m_Context->GetDeviceVK(), imageView, allocator);
		vkFreeMemory(m_Context->GetDeviceVK(), imageMemory, allocator);

		m_Image = VK_NULL_HANDLE;
		imageView = VK_NULL_HANDLE;
		imageMemory = VK_NULL_HANDLE;
	}

	void VulkanImage::SetData(BinaryBuffer& buffer)
	{
		if (!m_StagingBuffer.IsValid())
		{
			m_StagingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, buffer.GetSize());
			auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
			stagingBuffer->AllocateMemory();
		}

		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(m_StagingBuffer);
		stagingBuffer->SetMemory(buffer.GetSize(), buffer.GetData().data());


		ResourceID commandPoolID = m_Context->GetCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToImage(m_StagingBuffer, m_ResourceID, m_Width, m_Height);
		commandBuffer->EndCommands();
		commandBuffer->Flush();
		commandPool->ReleaseBuffer(commandBufferID);
	}

	VkImageMemoryBarrier VulkanImage::CreateMemoryBarrier(ResourceID imageID, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage)
	{
		auto image = ResourceManager::GetResource<VulkanImage>(imageID);
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image->GetImage();
		barrier.subresourceRange.aspectMask = image->isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		return barrier;
	}

	uint32_t VulkanImage::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
	VkImageUsageFlags VulkanImage::GetUsage(ImageType imageType)
	{
		switch (imageType)
		{
			case Odyssey::ImageType::None:
			case Odyssey::ImageType::Image2D:
				return VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			case Odyssey::ImageType::RenderTexture:
				return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			case ImageType::DepthTexture:
				return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			default:
				return 0;
		}
	}
	VkFormat VulkanImage::GetFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::None:
			case TextureFormat::R8G8B8_UNORM:
				return VK_FORMAT_R8G8B8_SRGB;
			case TextureFormat::R8G8B8A8_SRGB:
				return VK_FORMAT_R8G8B8A8_SRGB;
			case TextureFormat::R8G8B8A8_UNORM:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::D24_UNORM_S8_UINT:
				return VK_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::D32_SFLOAT:
				return VK_FORMAT_D32_SFLOAT;
			case TextureFormat::D32_SFLOAT_S8_UINT:
				return VK_FORMAT_D32_SFLOAT_S8_UINT;
			default:
				return VK_FORMAT_R8G8B8A8_UNORM;
				break;
		}
	}
	bool VulkanImage::IsDepthFormat(TextureFormat format)
	{
		return format == TextureFormat::D32_SFLOAT || format == TextureFormat::D32_SFLOAT_S8_UINT ||
			format == TextureFormat::D24_UNORM_S8_UINT;
	}
}