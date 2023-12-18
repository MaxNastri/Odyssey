#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanImage.h"
#include "ResourceManager.h"

namespace Odyssey
{
    VulkanSwapchain::VulkanSwapchain(std::shared_ptr<VulkanContext> context, VulkanSurface* surface)
    {
        m_Context = context;
        CreateSwapchain(surface);
        CreateSwapchainImages(surface->GetSurfaceFormat().format);
    }

	void VulkanSwapchain::Destroy()
	{
        vkDestroySwapchainKHR(m_Context->GetDeviceVK(), swapchain, allocator);
        swapchain = VK_NULL_HANDLE;
        imageCount = 0;
	}

    std::vector<ResourceHandle<VulkanImage>> VulkanSwapchain::GetBackbuffers()
    {
        return backbuffers;
    }

    void VulkanSwapchain::CreateSwapchain(VulkanSurface* surface)
	{
        VkResult err = vkDeviceWaitIdle(m_Context->GetDeviceVK());
        if (!check_vk_result(err))
        {
            Logger::LogError("(Swapchain 1)");
        }

		minImageCount = GetMinImageCount(surface->GetPresentMode());
        VkSurfaceFormatKHR surfaceFormat = surface->GetSurfaceFormat();
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
        err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->GetPhysicalDeviceVK(), vkSurface, &cap);
        if (!check_vk_result(err))
        {
            Logger::LogError("(Swapchain 2)");
        }
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

        err = vkCreateSwapchainKHR(m_Context->GetDeviceVK(), &info, allocator, &swapchain);
        if (!check_vk_result(err))
        {
            Logger::LogError("(Swapchain 3)");
        }

        err = vkGetSwapchainImagesKHR(m_Context->GetDeviceVK(), swapchain, &imageCount, nullptr);
        if (!check_vk_result(err))
        {
            Logger::LogError("(Swapchain 4)");
        }
	}

    void VulkanSwapchain::CreateSwapchainImages(VkFormat format)
    {
        // Get the swapchain backbuffers as raw vkimages
        std::vector<VkImage> backbufferImages;
        backbufferImages.resize(imageCount);
        if (vkGetSwapchainImagesKHR(m_Context->GetDeviceVK(), swapchain, &imageCount, backbufferImages.data()) != VK_SUCCESS)
        {
            Logger::LogError("(VulkanSwapchain) Cannot retrieve swapchain images.");
            return;
        }

        backbuffers.resize(backbufferImages.size());
        for (uint16_t i = 0; i < backbuffers.size(); ++i)
        {
            backbuffers[i] = ResourceManager::AllocateImage(backbufferImages[i], format);
        }
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