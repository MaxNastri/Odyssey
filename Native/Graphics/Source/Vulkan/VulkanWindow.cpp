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

	void VulkanWindow::SetupFrameData(VulkanSwapchain* swapchain)
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

	void VulkanWindow::PreRender()
	{
	}

	bool VulkanWindow::BeginFrame(VulkanFrame*& currentFrame)
	{
		VkResult err;
		VkDevice vkDevice = m_Context->GetDevice()->GetLogicalDevice();

		const VkSemaphore* image_acquired_semaphore = frames[frameIndex].GetImageAcquiredSemaphore();
		const VkSemaphore* render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();

		VulkanFrame& frame = frames[frameIndex];

		// Wait for the initial fences to clear
		err = vkWaitForFences(vkDevice, 1, &(frame.fence), VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(vkDevice, 1, &frame.fence);
		check_vk_result(err);

		frame.commandPool.Reset(m_Context->GetDevice());

		// Command buffer begin
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(frame.commandBuffer, &info);
		check_vk_result(err);

		currentFrame = &frame;
		return true;
	}

	void VulkanWindow::UpdateFrameIndex(uint32_t imageCount)
	{
		frameIndex = (frameIndex + 1) % imageCount; // Now we can use the next set of semaphores
	}

	void VulkanWindow::SetRenderPass(VulkanSwapchain* swapchain, std::shared_ptr<VulkanRenderPass> renderPass)
	{
		frameBufferPass = renderPass;
		SetupFrameData(swapchain);
	}

	const VkSemaphore* VulkanWindow::GetRenderComplete()
	{
		return frames[frameIndex].GetRenderCompleteSemaphore();
	}

	const VkSemaphore* VulkanWindow::GetImageAcquired()
	{
		return frames[frameIndex].GetImageAcquiredSemaphore();
	}

	void VulkanWindow::Resize(VulkanSwapchain* swapchain)
	{
		VulkanDevice* device = m_Context->GetDevice();

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
			ImGui_ImplVulkan_SetMinImageCount(swapchain->minImageCount);
			frameIndex = 0;

			// Remake the frame data with the new size
			SetupFrameData(swapchain);
		}
	}
}