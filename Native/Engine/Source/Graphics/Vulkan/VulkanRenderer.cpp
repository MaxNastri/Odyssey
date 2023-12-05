#include "VulkanRenderer.h"
#include "VulkanGlobals.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanWindow.h"
#include "VulkanBuffer.h"
#include "VulkanQueue.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"
#include "ResourceManager.h"
#include "PerFrameRenderingData.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Material.h"
#include "VulkanDescriptorBuffer.h"
#include <chrono>

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		context = std::make_shared<VulkanContext>();
		ResourceManager::Initialize(context);
		context->SetupResources();

		window = std::make_shared<VulkanWindow>(context);
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());

		// IMGUI
		VulkanImgui::InitInfo imguiInfo = CreateImguiInitInfo();
		imgui = std::make_shared<VulkanImgui>(context, imguiInfo);

		renderingData = std::make_shared<PerFrameRenderingData>();

		// Drawing
		SetupFrameData();

		// Render scenes
		renderScenes.resize(frames.size());
		for (int i = 0; i < renderScenes.size(); i++)
		{
			renderScenes[i] = std::make_shared<RenderScene>();
		}

		for (int i = 0; i < frames.size(); ++i)
		{
			commandPools.push_back(ResourceManager::AllocateCommandPool());
			commandBuffers.push_back(commandPools[i].Get()->AllocateBuffer());
		}
	}

	void VulkanRenderer::Destroy()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		for (int i = 0; i < frames.size(); ++i)
		{
			frames[i].Destroy();
		}

		frames.clear();
		swapchain.reset();
		window.reset();

		context->Destroy();
		context.reset();
	}

	bool VulkanRenderer::Update()
	{
		return window->Update();
	}

	bool VulkanRenderer::Render()
	{
		if (rebuildSwapchain)
			RebuildSwapchain();

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
		VkResult err = vkQueuePresentKHR(context->GetGraphicsQueueVK(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			rebuildSwapchain = true;
			return false;
		}

		if (!check_vk_result(err))
		{
			Logger::LogError("(renderer 1)");
		}
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

		if (!check_vk_result(err))
		{
			Logger::LogError("(renderer 2)");
		}

		VulkanFrame& frame = frames[frameIndex];

		// Wait for the initial fences to clear
		err = vkWaitForFences(vkDevice, 1, &(frame.fence), VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking

		if (!check_vk_result(err))
		{
			Logger::LogError("(renderer 3)");
		}

		err = vkResetFences(vkDevice, 1, &frame.fence);

		if (!check_vk_result(err))
		{
			Logger::LogError("(renderer 4)");
		}

		// Clear resources for this frame index
		ResourceManager::FlushDestroys(frameIndex);
		commandPools[frameIndex].Get()->Reset();

		// Command buffer begin
		commandBuffers[frameIndex].Get()->BeginCommands();

		// Transition the swapchain image back to a format for writing
		commandBuffers[frameIndex].Get()->TransitionLayouts(frame.GetRenderTarget(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		currentFrame = &frame;
		return true;
	}

	void VulkanRenderer::RenderFrame()
	{
		VulkanFrame* frame = nullptr;
		if (BeginFrame(frame))
		{
			unsigned int width = window->GetSurface()->GetWidth();
			unsigned int height = window->GetSurface()->GetHeight();

			renderScenes[frameIndex]->ConvertScene(SceneManager::GetActiveScene());

			// RenderPass begin
			VulkanCommandBuffer* commandBuffer = commandBuffers[frameIndex].Get();
			renderingData->frame = frame;
			renderingData->renderScene = renderScenes[frameIndex];
			renderingData->width = width;
			renderingData->height = height;

			RenderPassParams params;
			params.commandBuffer = commandBuffers[frameIndex];
			params.context = context;
			params.renderingData = renderingData;

			renderPasses[1]->SetRenderTarget(frame->GetRenderTarget());

			for (const auto& renderPass : renderPasses)
			{
				renderPass->BeginPass(params);
				renderPass->Execute(params);
				renderPass->EndPass(params);
			}

			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = frame->GetImageAcquiredSemaphore();
			submitInfo.pWaitDstStageMask = &wait_stage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandBuffer->GetCommandBufferRef();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = frame->GetRenderCompleteSemaphore();

			commandBuffer->EndCommands();
			VkResult err = vkQueueSubmit(context->GetGraphicsQueueVK(), 1, &submitInfo, frame->fence);
			if (!check_vk_result(err))
			{
				Logger::LogError("(graphnode 1)");
			}
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		window->Resize(swapchain.get());

		// Remake the swapchain
		swapchain->Destroy();
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		rebuildSwapchain = false;

		// Destroy the existing frames
		for (auto& frame : frames)
		{
			frame.Destroy();
		}
		frames.clear();

		// Remake the frame data with the new size
		SetupFrameData();
	}

	VulkanImgui::InitInfo VulkanRenderer::CreateImguiInitInfo()
	{
		VulkanImgui::InitInfo info;
		info.window = window->GetWindow()->GetWindowHandle();
		info.instance = context->GetInstance();
		info.physicalDevice = context->GetPhysicalDevice()->GetPhysicalDevice();
		info.logicalDevice = context->GetDevice()->GetLogicalDevice();
		info.queueIndex = context->GetPhysicalDevice()->GetFamilyIndex(VulkanQueueType::Graphics);
		info.queue = context->GetGraphicsQueueVK();
		info.descriptorPool = nullptr;
		info.renderPass = nullptr;
		info.minImageCount = swapchain->minImageCount;
		info.imageCount = swapchain->imageCount;
		info.colorFormat = window->GetSurface()->GetFormat();
		return info;
	}

	void VulkanRenderer::SetupFrameData()
	{
		frameIndex = 0;

		VkDevice vkDevice = context->GetDevice()->GetLogicalDevice();
		VkFormat format = window->GetSurface()->GetFormat();

		// Create the frames
		{
			std::vector<std::shared_ptr<VulkanImage>> backbuffers = swapchain->GetBackbuffers();
			uint32_t imageCount = swapchain->GetImageCount();
			frames.resize(imageCount);

			for (uint32_t i = 0; i < imageCount; ++i)
			{
				frames[i] = VulkanFrame(context, backbuffers[i], format);
			}
		}

	}
}