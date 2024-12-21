#pragma once
#include "VulkanGlobals.h"
#include "VulkanCommandPool.h"
#include "VulkanSurface.h"

VK_FWD_DECLARE(VkFence)
VK_FWD_DECLARE(VkImage)
VK_FWD_DECLARE(VkImageView)
VK_FWD_DECLARE(VkSemaphore)

namespace Odyssey
{
	class VulkanContext;

	class VulkanFrame
	{
	public:
		VulkanFrame() = default;
		VulkanFrame(std::shared_ptr<VulkanContext> context, ResourceID colorTexture, VkFormat format);

	public:
		void Destroy();
		
	public:
		void SetColorTexture(ResourceID colorTexture, uint32_t imageIndex);

	public:
		const VkSemaphore* GetImageAcquiredSemaphore() { return &imageAcquiredSemaphore; }
		const VkSemaphore* GetRenderCompleteSemaphore() { return &renderCompleteSemaphore; }
		VkFence GetFence() { return fence; }
		ResourceID GetFrameTexture() { return m_FrameTexture; }

	private:
		void CreateFence();
		void CreateSempaphores();

	public:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceID m_FrameTexture;
		uint32_t m_ImageIndex;
		VkFence fence = VK_NULL_HANDLE;

	private:
		VkSemaphore imageAcquiredSemaphore;
		VkSemaphore renderCompleteSemaphore;
	};
}