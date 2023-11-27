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
	class VulkanImage;

	class VulkanFrame
	{
	public:
		VulkanFrame() = default;
		VulkanFrame(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanImage> renderTarget, VkFormat format);

	public:
		void Destroy();
		
	public:
		const VkSemaphore* GetImageAcquiredSemaphore() { return &imageAcquiredSemaphore; }
		const VkSemaphore* GetRenderCompleteSemaphore() { return &renderCompleteSemaphore; }
		VkFence GetFence() { return fence; }
		VulkanImage* GetRenderTarget() { return m_RenderTarget.get(); }
		VkImage GetRenderTargetVK();
		VkImageView GetRenderTargetViewVK();

	private:
		void CreateFence();
		void CreateSempaphores();

	public:
		std::shared_ptr<VulkanContext> m_Context;
		std::shared_ptr<VulkanImage> m_RenderTarget;
		VkFence fence = VK_NULL_HANDLE;

	private:
		VkSemaphore imageAcquiredSemaphore;
		VkSemaphore renderCompleteSemaphore;
	};
}