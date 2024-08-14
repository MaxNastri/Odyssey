#include "VulkanSurface.h"
#include "VulkanGlobals.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include <glfw3.h>

namespace Odyssey
{
	VulkanSurface::VulkanSurface(VulkanContext* context, GLFWwindow* window)
	{
		VulkanPhysicalDevice* physicalDevice = context->GetPhysicalDevice();
		uint32_t graphicsIndex = physicalDevice->GetFamilyIndex(VulkanQueueType::Graphics);

		VkResult err = glfwCreateWindowSurface(context->GetInstance(), window, allocator, &surface);
		if (!check_vk_result(err))
		{
			Logger::LogError("(surface 1)");
		}

		glfwGetFramebufferSize(window, &width, &height);

		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->GetPhysicalDevice(), graphicsIndex, surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] =
		{
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_B8G8R8_UNORM,
			VK_FORMAT_R8G8B8_UNORM
		};

		// Select a format
		int arrayLength = (int)(sizeof(requestSurfaceImageFormat) / sizeof(requestSurfaceImageFormat[0]));
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		format = SelectSurfaceFormat(physicalDevice->GetPhysicalDevice(), surface, requestSurfaceImageFormat, arrayLength, requestSurfaceColorSpace);

#ifdef ALLOW_UNCAPPED_FRAMERATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		// Select Present Mode
		int presentModesLength = (int)(sizeof(present_modes) / sizeof(present_modes[0]));
		presentMode = SelectPresentMode(physicalDevice->GetPhysicalDevice(), surface, &present_modes[0], presentModesLength);
	}

	void VulkanSurface::SetFrameBufferSize(int w, int h)
	{
		width = w;
		height = h;
	}

	VkSurfaceFormatKHR VulkanSurface::SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space)
	{
		assert(request_formats != nullptr);
		assert(request_formats_count > 0);

		// Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
		// Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
		// Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
		// hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.
		uint32_t avail_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, nullptr);

		std::vector<VkSurfaceFormatKHR> avail_format;
		avail_format.resize((int)avail_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, avail_format.data());

		// First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
		if (avail_count == 1)
		{
			if (avail_format[0].format == VK_FORMAT_UNDEFINED)
			{
				VkSurfaceFormatKHR ret;
				ret.format = request_formats[0];
				ret.colorSpace = request_color_space;
				return ret;
			}
			else
			{
				// No point in searching another format
				return avail_format[0];
			}
		}
		else
		{
			// Request several formats, the first found will be used
			for (int request_i = 0; request_i < request_formats_count; request_i++)
			{
				for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
				{
					if (avail_format[avail_i].format == request_formats[request_i] &&
						avail_format[avail_i].colorSpace == request_color_space)
					{
						return avail_format[avail_i];
					}
				}
			}

			// If none of the requested image formats could be found, use the first available
			return avail_format[0];
		}
	}

	VkPresentModeKHR VulkanSurface::SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count)
	{
		assert(request_modes != nullptr);
		assert(request_modes_count > 0);

		// Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
		uint32_t avail_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, nullptr);

		std::vector<VkPresentModeKHR> avail_modes;
		avail_modes.resize((int)avail_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, avail_modes.data());

		for (int request_i = 0; request_i < request_modes_count; request_i++)
		{
			for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
			{
				if (request_modes[request_i] == avail_modes[avail_i])
				{
					return request_modes[request_i];
				}
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR; // Always available
	}
}