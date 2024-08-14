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
	class VulkanRenderTexture;

	class VulkanFrame
	{
	public:
		VulkanFrame() = default;
		VulkanFrame(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanRenderTexture> renderTexture, VkFormat format);

	public:
		void Destroy();
		
	public:
		void SetRenderTarget(ResourceHandle<VulkanRenderTexture> renderTexture, uint32_t imageIndex);

	public:
		const VkSemaphore* GetImageAcquiredSemaphore() { return &imageAcquiredSemaphore; }
		const VkSemaphore* GetRenderCompleteSemaphore() { return &renderCompleteSemaphore; }
		VkFence GetFence() { return fence; }
		ResourceHandle<VulkanRenderTexture> GetRenderTarget() { return m_RenderTexture; }
		VkImage GetRenderTargetVK();
		VkImageView GetRenderTargetViewVK();

	private:
		void CreateFence();
		void CreateSempaphores();

	public:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceHandle<VulkanRenderTexture> m_RenderTexture;
		uint32_t m_ImageIndex;
		VkFence fence = VK_NULL_HANDLE;
	private:
		VkSemaphore imageAcquiredSemaphore;
		VkSemaphore renderCompleteSemaphore;
	};
}