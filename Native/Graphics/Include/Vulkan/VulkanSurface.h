#pragma once
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Odyssey
{
	class VulkanSurface
	{
	public:
		VulkanSurface() = default;
		VulkanSurface(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t graphicsQueueIndex, GLFWwindow* window);

	public:
		void SetFrameBufferSize(int width, int height);

	public:
		VkSurfaceKHR GetVK() { return surface; }
		VkSurfaceFormatKHR GetFormat() { return format; }
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