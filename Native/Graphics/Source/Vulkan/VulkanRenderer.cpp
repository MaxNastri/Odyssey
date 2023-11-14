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
		graphicsQueue = std::make_unique<VulkanQueue>(VulkanQueueType::Graphics, context->GetPhysicalDevice(), context->GetDevice());
		descriptorPool = std::make_unique<VulkanDescriptorPool>(context->GetDevice());
		renderPass = std::make_shared<VulkanRenderPass>(context->GetDevice(), window->GetSurface()->GetFormat());

		window->SetRenderPass(renderPass);

		VulkanImgui::InitInfo info = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(context.get(), info);
	}

	bool VulkanRenderer::Update()
	{
		return window->Update();
	}

	bool VulkanRenderer::Render()
	{
		window->PreRender(context.get());

		imgui->SubmitDraws();
		RenderFrame();
		imgui->PostRender();

		window->Present(graphicsQueue.get());
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
		if (window->BeginFrame(context.get(), frame))
		{
			int width = window->GetSurface()->GetWidth();
			int height = window->GetSurface()->GetHeight();

			// RenderPass begin
			renderPass->Begin(frame->commandBuffer, frame->framebuffer, width, height, ClearValue);

			// TODO: DRAW
			imgui->Render(frame->commandBuffer);

			// RenderPass end
			renderPass->End(frame->commandBuffer);

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
		info.minImageCount = window->GetSwapchain()->minImageCount;
		info.imageCount = window->GetSwapchain()->imageCount;
		return info;
	}
}