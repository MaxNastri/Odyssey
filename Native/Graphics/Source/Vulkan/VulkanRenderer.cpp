#include "VulkanRenderer.h"
#include "VulkanGlobals.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		window = std::make_unique<Window>();
		context = std::make_shared<VulkanContext>();
		graphicsQueue = std::make_unique<VulkanQueue>(VulkanQueueType::Graphics, context->GetPhysicalDevice(), context->GetDevice());
		descriptorPool = std::make_unique<VulkanDescriptorPool>(context->GetDevice());
		surface = std::make_unique<VulkanSurface>(context.get(), window->GetWindowHandle());
		renderPass = std::make_unique<VulkanRenderPass>(context->GetDevice(), surface->GetFormat().format);
		swapchain = std::make_unique<VulkanSwapchain>(context.get(), surface.get());

		VulkanImgui::InitInfo info = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(context.get(), info);

		SetupFrameData();
	}

	bool VulkanRenderer::Update()
	{
		return window->Update();
	}

	bool VulkanRenderer::Render()
	{
		// if rebuild swapchain, do it
		if (rebuildSwapchain)
		{
			RebuildSwapchain();
		}

		imgui->SubmitDraws();
		RenderFrame();
		imgui->PostRender();
		Present();
		return true;
	}

	void VulkanRenderer::RenderFrame()
	{
		VkResult err;
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		VkClearValue ClearValue;
		ClearValue.color.float32[0] = 0.0f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 0.0f;

		VkSemaphore image_acquired_semaphore = frames[frameIndex].GetImageAcquiredSemaphore();
		VkSemaphore render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();
		err = vkAcquireNextImageKHR(vkDevice, swapchain->GetVK(), UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &frameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return;
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

		// RenderPass begin
		renderPass->Begin(frame.commandBuffer, frame.framebuffer, surface->GetWidth(), surface->GetHeight(), ClearValue);

		// TODO: DRAW
		imgui->Render(frame.commandBuffer);

		// RenderPass end
		renderPass->End(frame.commandBuffer);

		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &image_acquired_semaphore;
		submitInfo.pWaitDstStageMask = &wait_stage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame.commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(frame.commandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(graphicsQueue->queue, 1, &submitInfo, frame.fence);
		check_vk_result(err);
	}

	void VulkanRenderer::Present()
	{
		if (rebuildSwapchain)
			return;

		VkSemaphore render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
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

	void VulkanRenderer::SetupFrameData()
	{
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		VkFormat format = surface->GetFormat().format;

		std::vector<VkImage> backbuffers = swapchain->GetBackbuffers(context->GetDevice());
		uint32_t imageCount = swapchain->GetImageCount();
		frames.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			frames[i] = VulkanFrame(context->GetDevice(), context->GetPhysicalDevice());
			frames[i].SetBackbuffer(context->GetDevice(), backbuffers[i], format);
			frames[i].CreateFramebuffer(context->GetDevice(), renderPass.get(), surface->GetWidth(), surface->GetHeight());
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		for (auto& frame : frames)
		{
			frame.Destroy(device);
		}
		frames.clear();

		int width, height;
		window->GetFrameBufferSize(width, height);
		surface->SetFrameBufferSize(width, height);

		if (width > 0 && height > 0)
		{
			swapchain->Destroy(device);
			swapchain.reset();
			swapchain = std::make_unique<VulkanSwapchain>(context.get(), surface.get());

			ImGui_ImplVulkan_SetMinImageCount(swapchain->minImageCount);
			frameIndex = 0;
			rebuildSwapchain = false;

			SetupFrameData();
		}
	}
	
	VulkanImgui::InitInfo VulkanRenderer::CreateImguiInitInfo()
	{
		VulkanImgui::InitInfo info;
		info.window = window->GetWindowHandle();
		info.instance = context->GetInstance();
		info.physicalDevice = context->GetPhysicalDevice()->GetPhysicalDevice();
		info.logicalDevice = context->GetDevice()->GetLogicalDevice();
		info.queueIndex = context->GetPhysicalDevice()->GetFamilyIndex(VulkanQueueType::Graphics);
		info.queue = graphicsQueue->queue;
		info.descriptorPool = descriptorPool->descriptorPool;
		info.renderPass = renderPass->GetVK();
		info.minImageCount = swapchain->minImageCount;
		info.imageCount = swapchain->imageCount;
		return info;
	}
}