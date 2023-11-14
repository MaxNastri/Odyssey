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
		swapchain = std::make_unique<VulkanSwapchain>(context.get(), surface.get());

	}

	void VulkanWindow::SetupFrameData()
	{
		VkDevice vkDevice = m_Context->GetDevice()->GetLogicalDevice();
		VkFormat format = surface->GetFormat();

		std::vector<VkImage> backbuffers = swapchain->GetBackbuffers(m_Context->GetDevice());
		uint32_t imageCount = swapchain->GetImageCount();
		frames.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			frames[i] = VulkanFrame(m_Context->GetDevice(), m_Context->GetPhysicalDevice());
			frames[i].SetBackbuffer(m_Context->GetDevice(), backbuffers[i], format);
			frames[i].CreateFramebuffer(m_Context->GetDevice(), frameBufferPass.get(), surface->GetWidth(), surface->GetHeight());
		}
	}
	bool VulkanWindow::Update()
	{
		return window->Update();
	}

	void VulkanWindow::PreRender(VulkanContext* context)
	{
		if (rebuildSwapchain)
		{
			RebuildSwapchain(context);
		}
	}

	bool VulkanWindow::BeginFrame(VulkanContext* context, VulkanFrame*& currentFrame)
	{
		VkResult err;
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();

		const VkSemaphore* image_acquired_semaphore = frames[frameIndex].GetImageAcquiredSemaphore();
		const VkSemaphore* render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();
		err = vkAcquireNextImageKHR(vkDevice, swapchain->GetVK(), UINT64_MAX, *image_acquired_semaphore, VK_NULL_HANDLE, &frameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return false;
		}

		check_vk_result(err);

		VulkanFrame& frame = frames[frameIndex];

		// Wait for the initial fences to clear
		err = vkWaitForFences(vkDevice, 1, &(frame.fence), VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(vkDevice, 1, &frame.fence);
		check_vk_result(err);

		frame.commandPool.Reset(context->GetDevice());

		// Command buffer begin
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(frame.commandBuffer, &info);
		check_vk_result(err);

		currentFrame = &frame;
		return true;
	}

	void VulkanWindow::Present(VulkanQueue* graphicsQueue)
	{
		if (rebuildSwapchain)
			return;

		const VkSemaphore* render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &swapchain->swapchain;
		info.pImageIndices = &frameIndex;
		VkResult err = vkQueuePresentKHR(graphicsQueue->queue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return;
		}
		check_vk_result(err);
		frameIndex = (frameIndex + 1) % swapchain->imageCount; // Now we can use the next set of semaphores
	}

	void VulkanWindow::SetRenderPass(std::shared_ptr<VulkanRenderPass> renderPass)
	{
		frameBufferPass = renderPass;
		SetupFrameData();
	}

	void VulkanWindow::RebuildSwapchain(VulkanContext* context)
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		// Destroy the existing frames
		for (auto& frame : frames)
		{
			frame.Destroy(device);
		}
		frames.clear();

		// Re-set the frame buffer size
		int width, height;
		window->GetFrameBufferSize(width, height);
		surface->SetFrameBufferSize(width, height);

		if (width > 0 && height > 0)
		{
			// Remake the swapchain
			swapchain->Destroy(device);
			swapchain.reset();
			swapchain = std::make_unique<VulkanSwapchain>(context, surface.get());

			ImGui_ImplVulkan_SetMinImageCount(swapchain->minImageCount);
			frameIndex = 0;
			rebuildSwapchain = false;

			// Remake the frame data with the new size
			SetupFrameData();
		}
	}
}