#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkSwapchainKHR)
VK_FWD_DECLARE(VkDevice)
VK_FWD_DECLARE(VkPhysicalDevice)

namespace Odyssey
{
	class VulkanContext;
	class VulkanDevice;
	class VulkanSurface;
	class VulkanImage;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(std::shared_ptr<VulkanContext> context, VulkanSurface* surface);
		void Destroy();

	public:
		uint32_t GetImageCount() { return imageCount; }
		std::vector<std::shared_ptr<VulkanImage>> GetBackbuffers();
		VkSwapchainKHR GetVK() { return swapchain; }

	private:
		void CreateSwapchain(VulkanSurface* surface);
		void CreateSwapchainImages(VkFormat surfaceFormat);
		int GetMinImageCount(VkPresentModeKHR presentMode);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkSwapchainKHR swapchain;
		std::vector<std::shared_ptr<VulkanImage>> backbuffers;
		uint32_t imageCount;
		uint32_t minImageCount;
	};
}