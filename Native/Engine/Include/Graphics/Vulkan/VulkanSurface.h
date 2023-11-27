#pragma once
#include "volk.h"
#include "VulkanGlobals.h"


VK_FWD_DECLARE(VkSurfaceKHR)

struct GLFWwindow;

namespace Odyssey
{
	class VulkanContext;

	class VulkanSurface
	{
	public:
		VulkanSurface() = default;
		VulkanSurface(VulkanContext* context, GLFWwindow* window);

	public:
		void SetFrameBufferSize(int width, int height);

	public:
		VkSurfaceKHR GetVK() { return surface; }
		VkFormat GetFormat() { return format.format; }
		VkSurfaceFormatKHR GetSurfaceFormat() { return format; }
		VkPresentModeKHR GetPresentMode() { return presentMode; }
		int GetWidth() { return width; }
		int GetHeight() { return height; }

	private:
		VkSurfaceFormatKHR SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space);
		VkPresentModeKHR SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count);
	
	private:
		VkSurfaceKHR surface;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		int width, height;
	};
}