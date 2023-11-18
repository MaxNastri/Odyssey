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

		window->SetRenderPass(swapchain.get(), renderPass);

		VulkanImgui::InitInfo info = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(context.get(), info);
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

		auto semaphore = window->GetRenderComplete();
		uint32_t frameIndex = window->GetFrameIndex();

		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &swapchain->swapchain;
		info.pImageIndices = &frameIndex;
		VkResult err = vkQueuePresentKHR(graphicsQueue->queue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return false;
		}

		check_vk_result(err);

		window->UpdateFrameIndex(swapchain->imageCount);
		return true;
	}

	bool VulkanRenderer::BeginFrame(VulkanFrame*& currentFrame)
	{
		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		const VkSemaphore* imageAcquired = window->GetImageAcquired();
		uint32_t frameIndex = window->GetFrameIndex();

		VkResult err = vkAcquireNextImageKHR(vkDevice, swapchain->GetVK(), UINT64_MAX, *imageAcquired, VK_NULL_HANDLE, &frameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return false;
		}

		check_vk_result(err);
		return window->BeginFrame(currentFrame);
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
			renderPass->Begin(frame->commandBuffer, frame->framebuffer, width, height, ClearValue);

			// TODO: DRAW
			imgui->Render(frame->commandBuffer);

			// RenderPass end
			renderPass->End(frame->commandBuffer);

			// TODO: Move this into the context?
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = frame->GetImageAcquiredSemaphore();
			submitInfo.pWaitDstStageMask = &wait_stage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &frame->commandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = frame->GetRenderCompleteSemaphore();

			VkResult err = vkEndCommandBuffer(frame->commandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(graphicsQueue->queue, 1, &submitInfo, frame->fence);
			check_vk_result(err);
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		// Remake the swapchain
		swapchain->Destroy(device);
		swapchain.reset();
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		rebuildSwapchain = false;

		window->Resize(swapchain.get());
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
}