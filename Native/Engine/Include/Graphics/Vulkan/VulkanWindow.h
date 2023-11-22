#pragma once
#include "VulkanGlobals.h"
#include "VulkanSwapchain.h"
#include "VulkanSurface.h"
#include "VulkanFrame.h"
#include "Window.h"
#include <vulkan/vulkan.h>

VK_FWD_DECLARE(VkCommandBuffer)

namespace Odyssey
{
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanRenderPass;
	class VulkanQueue;

	class VulkanWindow
	{
	public:
		VulkanWindow(std::shared_ptr<VulkanContext> context);

	public:
		bool Update();
		void Resize(VulkanSwapchain* swapchain);

	public:
		VulkanSurface* GetSurface() { return surface.get(); }
		Window* GetWindow() { return window.get(); }
	private:

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::unique_ptr<VulkanSurface> surface;
		std::unique_ptr<Window> window;

	};
}