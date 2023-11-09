#include "VulkanSwapchain.h"
#include "VulkanGlobals.h"
#include <assert.h>

namespace Odyssey::Graphics
{
	VulkanSwapchain::VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VulkanSurface* surface)
	{
        CreateSwapchain(device, physicalDevice, surface);
	}

	void VulkanSwapchain::Destroy(VkDevice device)
	{
		VkResult err = vkDeviceWaitIdle(device);
		check_vk_result(err);

        vkDestroySwapchainKHR(device, swapchain, allocator);
        swapchain = VK_NULL_HANDLE;
        imageCount = 0;
	}

    std::vector<VkImage> VulkanSwapchain::GetBackbuffers(VkDevice device)
    {
        std::vector<VkImage> backbuffers;
        backbuffers.resize(imageCount);
        VkResult err = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, backbuffers.data());
        check_vk_result(err);
        return backbuffers;
    }

    void VulkanSwapchain::CreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VulkanSurface* surface)
	{
        VkResult err = vkDeviceWaitIdle(device);
        check_vk_result(err);

		int minImageCount = GetMinImageCount(surface->GetPresentMode());
        VkSurfaceFormatKHR surfaceFormat = surface->GetFormat();
        VkSurfaceKHR vkSurface = surface->GetVK();
        int width = surface->GetWidth();
        int height = surface->GetHeight();

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface->GetVK();
        info.minImageCount = minImageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;           // Assume that graphics family == present family
        info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = surface->GetPresentMode();
        info.clipped = VK_TRUE;
        info.oldSwapchain = nullptr;

        // Get the capabilities of the surface
        VkSurfaceCapabilitiesKHR cap;
        err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkSurface, &cap);
        check_vk_result(err);
        if (info.minImageCount < cap.minImageCount)
            info.minImageCount = cap.minImageCount;
        else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
            info.minImageCount = cap.maxImageCount;

        if (cap.currentExtent.width == 0xffffffff)
        {
            info.imageExtent.width = width;
            info.imageExtent.height = height;
        }
        else
        {
            info.imageExtent.width = width = cap.currentExtent.width;
            info.imageExtent.height = height = cap.currentExtent.height;
        }

        err = vkCreateSwapchainKHR(device, &info, allocator, &swapchain);
        check_vk_result(err);

        err = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        check_vk_result(err);
	}

	int VulkanSwapchain::GetMinImageCount(VkPresentModeKHR presentMode)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return 3;
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR || presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			return 2;
		if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			return 1;

		return 1;
	}
}