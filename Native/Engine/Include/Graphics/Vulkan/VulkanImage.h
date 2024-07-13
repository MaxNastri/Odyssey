#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "Resource.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	struct VulkanImageDescription
	{
		TextureType ImageType = TextureType::None;
		TextureFormat Format = TextureFormat::R8G8B8A8_UNORM;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Depth = 1;
		uint32_t MipLevels = 1;
		uint32_t ArrayLayers = 1;
		uint32_t Samples = 1;
	};

	class VulkanContext;
	class VulkanBuffer;

	class VulkanImage : public Resource
	{
	public:
		VulkanImage(std::shared_ptr<VulkanContext> context, VulkanImageDescription& desc);
		VulkanImage(std::shared_ptr<VulkanContext> context, VkImage image, uint32_t width, uint32_t height, VkFormat format);
		void Destroy();

	public:
		void SetData(const void* data);
		void SetLayout(VkImageLayout layout) { imageLayout = layout; }
	public:
		static VkImageMemoryBarrier CreateMemoryBarrier(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage);

	public:
		VkImage GetImage() { return m_Image; }
		VkImageView GetImageView() { return imageView; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		VkImageLayout GetLayout() { return imageLayout; }

	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkImageUsageFlags GetUsage(TextureType imageType);
		VkFormat GetFormat(TextureFormat format);
		bool IsDepthFormat(TextureFormat format);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkImage m_Image;
		VkImageView imageView;
		VkImageLayout imageLayout;
		VkDeviceMemory imageMemory;
		uint32_t m_Width, m_Height;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		bool isDepth = false;
	};
}