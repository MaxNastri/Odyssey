#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "Resource.h"
#include "BinaryBuffer.h"

VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	struct VulkanImageDescription
	{
		ImageType ImageType = ImageType::None;
		TextureFormat Format = TextureFormat::R8G8B8A8_UNORM;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Channels = 3;
		uint32_t Depth = 1;
		uint32_t ArrayDepth = 1;
		uint32_t Samples = 1;
		bool MipMapEnabled = false;
		float MipBias = 0.0f;
		uint32_t MaxMipCount = 0;
	};

	class VulkanContext;
	class VulkanBuffer;

	class VulkanImage : public Resource
	{
	public:
		VulkanImage(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanImageDescription& desc);
		VulkanImage(ResourceID id, std::shared_ptr<VulkanContext> context, VkImage image, uint32_t width, uint32_t height, uint32_t channels, VkFormat format);
		void Destroy();

	public:
		void SetData(BinaryBuffer& buffer);
		void SetData(BinaryBuffer& buffer, size_t arrayDepth);
		void SetLayout(VkImageLayout layout) { imageLayout = layout; }

	public:

	public:
		static VkImageMemoryBarrier CreateMemoryBarrier(ResourceID imageID, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage);

	public:
		VkImage GetImage() { return m_Image; }
		VkImageView GetImageView() { return imageView; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		uint32_t GetArrayDepth() { return m_ArrayDepth; }
		uint32_t GetMipLevels() { return m_MipLevels; }
		float GetMipBias() { return m_MipBias; }
		VkImageLayout GetLayout() { return imageLayout; }
		std::vector<VkBufferImageCopy> GetCopyRegions() { return m_CopyRegions; }

	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkImageUsageFlags GetUsage(ImageType imageType);
		VkFormat GetFormat(TextureFormat format);
		bool IsDepthFormat(TextureFormat format);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkImage m_Image;
		VkImageView imageView;
		VkImageLayout imageLayout;
		VkDeviceMemory imageMemory;
		std::vector<VkBufferImageCopy> m_CopyRegions;
		uint32_t m_Width, m_Height, m_Channels;
		uint32_t m_ArrayDepth;
		uint32_t m_MipLevels;
		float m_MipBias = 0.0f;
		ResourceID m_StagingBuffer;
		bool isDepth = false;
	};
}