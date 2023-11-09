#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "VulkanSurface.h"

namespace Odyssey::Graphics
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VulkanSurface* surface);
		void Destroy(VkDevice device);

	public:
		uint32_t GetImageCount() { return imageCount; }
		std::vector<VkImage> GetBackbuffers(VkDevice device);
		VkSwapchainKHR GetVK() { return swapchain; }

	private:
		void CreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VulkanSurface* surface);
		int GetMinImageCount(VkPresentModeKHR presentMode);

	public:
		VkSwapchainKHR swapchain;
		uint32_t imageCount;
	};
}