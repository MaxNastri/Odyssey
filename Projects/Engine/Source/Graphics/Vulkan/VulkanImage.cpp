#include "VulkanImage.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandPool.h"
#include "Vulkancommandbuffer.h"
#include <Log.h>
#include "ResourceManager.h"
#include "VulkanBuffer.h"

namespace Odyssey
{
	uint32_t GetMipCount(VulkanImageDescription& desc)
	{
		uint32_t mipCount = 1;

		if (desc.MipMapEnabled)
		{
			mipCount = (uint32_t)(std::floor(std::log2(std::max(desc.Width, desc.Height))) + 1);

			if (desc.MaxMipCount > 0)
				mipCount = std::min(mipCount, desc.MaxMipCount);
		}

		return mipCount;
	}

	VulkanImage::VulkanImage(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& desc)
		: Resource(id), m_ImageDesc(desc)
	{
		m_Context = context;
		m_MipLevels = GetMipCount(desc);

		isDepth = desc.ImageType == ImageType::DepthTexture || desc.ImageType == ImageType::Shadowmap;

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
			imageInfo.mipLevels = m_MipLevels;
			imageInfo.arrayLayers = desc.ArrayDepth;
			imageInfo.format = GetFormat(desc.Format);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = GetUsage(desc.ImageType);

			if (m_MipLevels > 1)
				imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (desc.ImageType == ImageType::RenderTexture || desc.ImageType == ImageType::DepthTexture)
				imageInfo.samples = (VkSampleCountFlagBits)desc.Samples;
			else
				imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

			if (desc.ImageType == ImageType::Cubemap)
				imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			VulkanAllocator allocator("Image");
			m_MemoryAllocation = allocator.AllocateImage(imageInfo, VMA_MEMORY_USAGE_AUTO, m_Image);
		}

		// Image view
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_Image;
			viewInfo.viewType = desc.ImageType == ImageType::Cubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = GetFormat(desc.Format);
			if (desc.ImageType == ImageType::Shadowmap)
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			else if (isDepth)
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			else
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			viewInfo.subresourceRange.levelCount = m_MipLevels;
			viewInfo.subresourceRange.layerCount = desc.ArrayDepth;

			if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				Log::Error("(VulkanImage) Failed to create image view.");
				return;
			}
		}

		for (size_t i = 0; i < m_ImageDesc.ArrayDepth; i++)
		{
			VkImageCopy copyRegion = {};
			copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.srcSubresource.mipLevel = 0;
			copyRegion.srcSubresource.baseArrayLayer = (uint32_t)i;
			copyRegion.srcSubresource.layerCount = 1;

			copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.dstSubresource.mipLevel = 0;
			copyRegion.dstSubresource.baseArrayLayer = (uint32_t)i;
			copyRegion.dstSubresource.layerCount = 1;

			copyRegion.extent.width = m_ImageDesc.Width;
			copyRegion.extent.height = m_ImageDesc.Height;
			copyRegion.extent.depth = 1;
			copyRegion.dstOffset = { 0, 0, 0 };
			copyRegion.srcOffset = { 0, 0, 0 };
			m_ImageCopyRegions.push_back(copyRegion);
		}

	}

	VulkanImage::VulkanImage(ResourceID id, std::shared_ptr<VulkanContext> context, VkImage image, uint32_t width, uint32_t height, uint32_t channels, VkFormat format)
		: Resource(id)
	{
		m_Context = context;
		m_Image = image;
		m_ImageDesc.Width = width;
		m_ImageDesc.Height = height;
		m_ImageDesc.Channels = channels;
		m_ImageDesc.ArrayDepth = 1;
		m_ImageDesc.MaxMipCount = 1;
		m_ImageDesc.Format = TextureFormat::R8G8B8A8_UNORM;
		isDepth = false;

		imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// Image view
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = m_ImageDesc.MaxMipCount;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(context->GetDeviceVK(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				Log::Error("(VulkanImage) Failed to create image view.");
				return;
			}
		}

		for (size_t i = 0; i < m_ImageDesc.ArrayDepth; i++)
		{
			VkImageCopy copyRegion = {};
			copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.srcSubresource.mipLevel = 0;
			copyRegion.srcSubresource.baseArrayLayer = (uint32_t)i;
			copyRegion.srcSubresource.layerCount = 1;

			copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.dstSubresource.mipLevel = 0;
			copyRegion.dstSubresource.baseArrayLayer = (uint32_t)i;
			copyRegion.dstSubresource.layerCount = 1;

			copyRegion.extent.width = m_ImageDesc.Width;
			copyRegion.extent.height = m_ImageDesc.Height;
			copyRegion.extent.depth = 1;
			copyRegion.dstOffset = { 0, 0, 0 };
			copyRegion.srcOffset = { 0, 0, 0 };
			m_ImageCopyRegions.push_back(copyRegion);
		}
	}

	void VulkanImage::Destroy()
	{
		VulkanAllocator allocator("Image");
		allocator.DestroyImage(m_Image, m_MemoryAllocation);
		vkDestroyImageView(m_Context->GetDeviceVK(), imageView, nullptr);

		m_Image = VK_NULL_HANDLE;
		imageView = VK_NULL_HANDLE;
	}

	void VulkanImage::SetData(BinaryBuffer& buffer)
	{
		// Set the staging buffer's memory
		ResourceID stagingBufferID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, buffer.GetSize());
		Ref<VulkanBuffer> stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(stagingBufferID);
		stagingBuffer->CopyData(buffer.GetSize(), buffer.GetData().data());

		// Generate a copy region for this new set of data
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = m_ImageDesc.Width;
		bufferCopyRegion.imageExtent.height = m_ImageDesc.Height;
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.imageOffset = { 0, 0, 0 };
		bufferCopyRegion.bufferOffset = 0;

		m_CopyRegions.clear();
		m_CopyRegions.push_back(bufferCopyRegion);

		// Allocate a command buffer
		ResourceID commandPoolID = m_Context->GetGraphicsCommandPool();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Copy the buffer into the image
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToImage(stagingBufferID, m_ResourceID, m_ImageDesc.Width, m_ImageDesc.Height);
		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();

		commandPool->ReleaseBuffer(commandBufferID);
		ResourceManager::Destroy(stagingBufferID);
	}

	void VulkanImage::SetData(BinaryBuffer& buffer, size_t arrayDepth)
	{
		size_t offset = buffer.GetSize() / arrayDepth;

		// Set the staging buffer's memory
		ResourceID stagingBufferID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, buffer.GetSize());
		Ref<VulkanBuffer> stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(stagingBufferID);
		stagingBuffer->CopyData(buffer.GetSize(), buffer.GetData().data());

		// Generate a copy region for this new set of data
		for (size_t i = 0; i < arrayDepth; i++)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = (uint32_t)i;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = m_ImageDesc.Width;
			bufferCopyRegion.imageExtent.height = m_ImageDesc.Height;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.imageOffset = { 0, 0, 0 };
			bufferCopyRegion.bufferOffset = offset * i;

			m_CopyRegions.push_back(bufferCopyRegion);
		}

		// Allocate a command buffer
		Ref<VulkanCommandPool> commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// Copy the buffer into the image
		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToImage(stagingBufferID, m_ResourceID, m_ImageDesc.Width, m_ImageDesc.Height);
		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();
		commandPool->ReleaseBuffer(commandBufferID);

		ResourceManager::Destroy(stagingBufferID);
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
		if (image->GetFormat() == TextureFormat::D24_UNORM_S8_UINT)
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		else if (image->GetFormat() == TextureFormat::D32_SFLOAT)
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = image->GetMipLevels();
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = image->GetArrayDepth();
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
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
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
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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
			case ImageType::None:
			case ImageType::Image2D:
			case ImageType::Image2DArray:
			case ImageType::Cubemap:
				return VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			case ImageType::RenderTexture:
				return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			case ImageType::DepthTexture:
				return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			case ImageType::Shadowmap:
				return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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
			case TextureFormat::R16G16B16_SFLOAT:
				return VK_FORMAT_R16G16B16_SFLOAT;
			case TextureFormat::R16G16B16A16_SFLOAT:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case TextureFormat::R32G32B32A32_SFLOAT:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			case TextureFormat::R16G16_SFLOAT:
				return VK_FORMAT_R16G16_SFLOAT;
			case TextureFormat::D24_UNORM_S8_UINT:
				return VK_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::D16_UNORM:
				return VK_FORMAT_D16_UNORM;
			case TextureFormat::D32_SFLOAT:
				return VK_FORMAT_D32_SFLOAT;
			case TextureFormat::D32_SFLOAT_S8_UINT:
				return VK_FORMAT_D32_SFLOAT_S8_UINT;
			default:
				return VK_FORMAT_R8G8B8A8_UNORM;
		}
	}
	bool VulkanImage::IsDepthFormat(TextureFormat format)
	{
		return format == TextureFormat::D32_SFLOAT || format == TextureFormat::D32_SFLOAT_S8_UINT ||
			format == TextureFormat::D24_UNORM_S8_UINT || format == TextureFormat::D16_UNORM;
	}
}