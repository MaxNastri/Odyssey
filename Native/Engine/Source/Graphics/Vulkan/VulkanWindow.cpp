#include "VulkanWindow.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "imgui_impl_vulkan.h"

namespace Odyssey
{
	VulkanWindow::VulkanWindow(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
		window = std::make_unique<Window>();
		surface = std::make_unique<VulkanSurface>(context.get(), window->GetWindowHandle());
	}

	bool VulkanWindow::Update()
	{
		return window->Update();
	}

	void VulkanWindow::Resize(VulkanSwapchain* swapchain)
	{
		// Re-set the frame buffer size
		int width, height;
		window->GetFrameBufferSize(width, height);
		surface->SetFrameBufferSize(width, height);

		if (width > 0 && height > 0)
		{
			ImGui_ImplVulkan_SetMinImageCount(swapchain->minImageCount);
		}
	}
}