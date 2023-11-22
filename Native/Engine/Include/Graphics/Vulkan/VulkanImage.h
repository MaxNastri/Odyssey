#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	enum class ImageType
	{
		None = 0,
		Image2D = 1,
		RenderTexture = 2,
	};

	enum class ImageFormat
	{
		None = 0,
		R8G8B8A8_SRGB = 1
	};

	enum class ImageTiling
	{
		None = 0,
		Optimal = 1,
		Linear = 2,
	};

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
		void SetData(VulkanBuffer* buffer, uint32_t width, uint32_t height);

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