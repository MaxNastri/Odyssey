#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	struct VulkanImageDescription
	{
		ImageType ImageType = ImageType::None;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Depth = 1;
		uint32_t MipLevels = 1;
		uint32_t ArrayLayers = 1;
		uint32_t Samples = 1;
	};

	class VulkanContext;
	class VulkanBuffer;

	class VulkanImage
	{
	public:
		VulkanImage(std::shared_ptr<VulkanContext> context, VulkanImageDescription& desc);
		VulkanImage(std::shared_ptr<VulkanContext> context, VkImage image, VkFormat format);
		void Destroy();

	public:
		void SetData(ResourceHandle<VulkanBuffer> handle, uint32_t width, uint32_t height);

	public:
		static VkImageMemoryBarrier CreateMemoryBarrier(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage);

	public:
		VkImage GetImage() { return m_Image; }
		VkImageView GetImageView() { return imageView; }

	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkImageUsageFlags GetUsage(ImageType imageType);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkImage m_Image;
		VkImageView imageView;
		VkImageLayout imageLayout;
		VkDeviceMemory imageMemory;
	};
}