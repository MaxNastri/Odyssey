#include "VulkanRenderer.h"
#include "VulkanGlobals.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanWindow.h"
#include "VulkanRenderPass.h"

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		context = std::make_shared<VulkanContext>();
		window = std::make_shared<VulkanWindow>(context);
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		graphicsQueue = std::make_unique<VulkanQueue>(VulkanQueueType::Graphics, context);
		descriptorPool = std::make_unique<VulkanDescriptorPool>(context->GetDevice());
		renderPass = std::make_shared<VulkanRenderPass>(context->GetDevice(), window->GetSurface()->GetFormat());

		VulkanImgui::InitInfo info = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(context.get(), info);

		SetupFrameData();
		for (int i = 0; i < frames.size(); ++i)
		{
			commandPool.push_back(std::make_unique<VulkanCommandPool>(context->GetDevice(), context->GetPhysicalDevice()->GetFamilyIndex(VulkanQueueType::Graphics)));
			commandBuffers.push_back(commandPool[i]->AllocateBuffer(context->GetDevice()));
		}
	}

	void VulkanRenderer::Destroy()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		for (int i = 0; i < frames.size(); ++i)
		{
			frames[i].Destroy(context->GetDevice());
		}
		frames.clear();

		renderPass.reset();
		descriptorPool.reset();
		graphicsQueue.reset();
		swapchain.reset();
		window.reset();
		context.reset();
	}

	bool VulkanRenderer::Update()
	{
		return window->Update();
	}

	bool VulkanRenderer::Render()
	{
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

	bool VulkanRenderer::Present()
	{
		if (rebuildSwapchain)
			return false;

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
		}

		check_vk_result(err);
		frameIndex = (frameIndex + 1) % swapchain->imageCount;
		return true;
	}

	bool VulkanRenderer::BeginFrame(VulkanFrame*& currentFrame)
	{
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		const VkSemaphore* imageAcquired = frames[frameIndex].GetImageAcquiredSemaphore();
		const VkSemaphore* render_complete_semaphore = frames[frameIndex].GetRenderCompleteSemaphore();

		VkResult err = vkAcquireNextImageKHR(vkDevice, swapchain->GetVK(), UINT64_MAX, *imageAcquired, VK_NULL_HANDLE, &frameIndex);
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

		commandPool[frameIndex]->Reset(context->GetDevice());

		// Command buffer begin
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(commandBuffers[frameIndex], &info);
		check_vk_result(err);

		currentFrame = &frame;
		return true;
	}

	void VulkanRenderer::RenderFrame()
	{
		VkClearValue ClearValue;
		ClearValue.color.float32[0] = 0.0f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 0.0f;

		VulkanFrame* frame = nullptr;
		if (BeginFrame(frame))
		{
			int width = window->GetSurface()->GetWidth();
			int height = window->GetSurface()->GetHeight();

			// RenderPass begin
			VkCommandBuffer commandBuffer = commandBuffers[frameIndex];
			renderPass->Begin(commandBuffer, frame->framebuffer, width, height, ClearValue);

			// TODO: DRAW
			imgui->Render(commandBuffer);

			// RenderPass end
			renderPass->End(commandBuffer);

			// TODO: Move this into the context?
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = frame->GetImageAcquiredSemaphore();
			submitInfo.pWaitDstStageMask = &wait_stage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = frame->GetRenderCompleteSemaphore();

			VkResult err = vkEndCommandBuffer(commandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(graphicsQueue->queue, 1, &submitInfo, frame->fence);
			check_vk_result(err);
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		window->Resize(swapchain.get());

		// Remake the swapchain
		swapchain->Destroy(device);
		swapchain.reset();
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		rebuildSwapchain = false;

		// Destroy the existing frames
		for (auto& frame : frames)
		{
			frame.Destroy(device);
		}
		frames.clear();

		// Remake the frame data with the new size
		SetupFrameData();
		frameIndex = 0;
	}

	VulkanImgui::InitInfo VulkanRenderer::CreateImguiInitInfo()
	{
		VulkanImgui::InitInfo info;
		info.window = window->GetWindow()->GetWindowHandle();
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

	void VulkanRenderer::SetupFrameData()
	{
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		VkFormat format = window->GetSurface()->GetFormat();

		std::vector<VkImage> backbuffers = swapchain->GetBackbuffers(context->GetDevice());
		uint32_t imageCount = swapchain->GetImageCount();
		frames.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			frames[i] = VulkanFrame(context->GetDevice(), context->GetPhysicalDevice());
			frames[i].SetBackbuffer(context->GetDevice(), backbuffers[i], format);
			frames[i].CreateFramebuffer(context->GetDevice(), renderPass.get(), window->GetSurface()->GetWidth(), window->GetSurface()->GetHeight());
		}
	}
}