#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "VulkanCommandPool.h"
#include "VulkanSurface.h"

namespace Odyssey
{
	class VulkanFrame
	{
	public:
		VulkanFrame(VkDevice device, uint32_t queueIndex);

	public:
		void Destroy(VkDevice device);
		void SetBackbuffer(VkDevice device, VkImage backbufferImage, VkFormat format);
		void CreateFramebuffer(VkDevice device, VkRenderPass renderPass, int width, int height);

		VkSemaphore GetImageAcquiredSemaphore() { return imageAcquiredSemaphore; }
		VkSemaphore GetRenderCompleteSemaphore() { return renderCompleteSemaphore; }
		VkFence GetFence() { return fence; }

	private:
		void CreateFence(VkDevice device);
		void CreateSempaphores(VkDevice device);

	public:
		std::unique_ptr<VulkanCommandPool> commandPool;
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