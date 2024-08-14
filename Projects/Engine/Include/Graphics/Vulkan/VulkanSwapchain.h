#pragma once
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkSwapchainKHR)
VK_FWD_DECLARE(VkDevice)
VK_FWD_DECLARE(VkPhysicalDevice)

namespace Odyssey
{
	class VulkanContext;
	class VulkanDevice;
	class VulkanSurface;
	class VulkanRenderTexture;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(std::shared_ptr<VulkanContext> context, VulkanSurface* surface);
		void Destroy();

	public:
		uint32_t GetImageCount() { return imageCount; }
		std::vector<ResourceHandle<VulkanRenderTexture>> GetBackbuffers();
		VkSwapchainKHR GetVK() { return swapchain; }

	private:
		void CreateSwapchain(VulkanSurface* surface);
		void CreateSwapchainImages(VkFormat surfaceFormat);
		int GetMinImageCount(VkPresentModeKHR presentMode);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkSwapchainKHR swapchain;
		std::vector<ResourceHandle<VulkanRenderTexture>> backbuffers;
		uint32_t imageCount;
		uint32_t minImageCount;
		uint32_t m_Width, m_Height;
	};
}