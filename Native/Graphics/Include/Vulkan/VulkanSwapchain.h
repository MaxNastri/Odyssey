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

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(std::shared_ptr<VulkanContext> context, VulkanSurface* surface);
		void Destroy(VulkanDevice* device);

	public:
		uint32_t GetImageCount() { return imageCount; }
		std::vector<VkImage> GetBackbuffers(VulkanDevice* device);
		VkSwapchainKHR GetVK() { return swapchain; }

	private:
		void CreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VulkanSurface* surface);
		int GetMinImageCount(VkPresentModeKHR presentMode);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkSwapchainKHR swapchain;
		uint32_t imageCount;
		uint32_t minImageCount;
	};
}