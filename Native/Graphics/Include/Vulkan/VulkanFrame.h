#pragma once
#include "VulkanGlobals.h"
#include "VulkanCommandPool.h"
#include "VulkanSurface.h"

VK_FWD_DECLARE(VkFence)
VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkImageView)
VK_FWD_DECLARE(VkFramebuffer)
VK_FWD_DECLARE(VkCommandBuffer)
VK_FWD_DECLARE(VkSemaphore)

namespace Odyssey
{
	class VulkanDevice;
	class VulkanPhysicalDevice;
	class VulkanRenderPass;

	class VulkanFrame
	{
	public:
		VulkanFrame() = default;
		VulkanFrame(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

	public:
		void Destroy(VulkanDevice* device);
		void SetBackbuffer(VulkanDevice* device, VkImage backbufferImage, VkFormat format);
		void CreateFramebuffer(VulkanDevice* device, VulkanRenderPass* renderPass, int width, int height);

		VkSemaphore GetImageAcquiredSemaphore() { return imageAcquiredSemaphore; }
		VkSemaphore GetRenderCompleteSemaphore() { return renderCompleteSemaphore; }
		VkFence GetFence() { return fence; }

	private:
		void CreateFence(VkDevice device);
		void CreateSempaphores(VkDevice device);

	public:
		VulkanCommandPool commandPool;
		VkFence fence = VK_NULL_HANDLE;
		VkImage backbuffer = VK_NULL_HANDLE;
		VkImageView backbufferView = VK_NULL_HANDLE;
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	private:
		VkSemaphore imageAcquiredSemaphore;
		VkSemaphore renderCompleteSemaphore;
	};
}