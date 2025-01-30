#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "Material.h"
#include "PerFrameRenderingData.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanQueue.h"
#include "VulkanRenderer.h"
#include "VulkanWindow.h"
#include "RenderTarget.h"
#include "VulkanTexture.h"
#include "VulkanAllocator.h"

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		m_Context = std::make_shared<VulkanContext>();

		VulkanAllocator::Init(m_Context);
		ResourceManager::Initialize(m_Context);
		m_Context->SetupResources();

		m_Window = std::make_shared<VulkanWindow>(m_Context);
		m_Swapchain = std::make_unique<VulkanSwapchain>(m_Context, m_Window->GetSurface());

		// IMGUI
		VulkanImgui::InitInfo imguiInfo = CreateImguiInitInfo();
		m_Imgui = std::make_shared<VulkanImgui>(m_Context, imguiInfo);

		// Set the default font for IMGUI
		float fontSize = std::floor(DEFAULT_FONT_SIZE * m_Window->GetWindow()->GetContentScale());
		m_Imgui->SetFont("Resources/Fonts/OpenSans/OpenSans-Regular.ttf", fontSize);

		m_RenderingData = std::make_shared<PerFrameRenderingData>();

		// Drawing
		SetupFrameData();

		// Render scenes
		m_RenderScenes.resize(m_Frames.size());
		for (int i = 0; i < m_RenderScenes.size(); i++)
		{
			m_RenderScenes[i] = std::make_shared<RenderScene>();
		}

		for (int i = 0; i < m_Frames.size(); ++i)
		{
			// Graphics command resources
			{
				ResourceID commandPoolID = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Graphics);
				auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(commandPoolID);
				ResourceID commandBufferID = commandPool->AllocateBuffer();

				m_GraphicsCommandPools.push_back(commandPoolID);
				m_GraphicsCommandBuffers.push_back(commandBufferID);
			}
		}
	}

	void VulkanRenderer::Destroy()
	{
		VulkanDevice* device = m_Context->GetDevice();
		device->WaitForIdle();

		for (int i = 0; i < m_Frames.size(); ++i)
		{
			m_Frames[i].Destroy();
		}

		m_Frames.clear();
		m_Swapchain.reset();
		m_Window.reset();

		m_Context->Destroy();
		m_Context.reset();
	}

	bool VulkanRenderer::Update()
	{
		m_Imgui->Update();
		return m_Window->Update();
	}

	bool VulkanRenderer::Render()
	{
		if (m_RebuildSwapchain)
			RebuildSwapchain();

		m_Imgui->SubmitDraws();
		RenderFrame();
		m_Imgui->PostRender();

		Present();
		return true;
	}

	bool VulkanRenderer::Present()
	{
		if (m_RebuildSwapchain)
			return false;

		const VkSemaphore* render_complete_semaphore = m_Frames[s_FrameIndex].GetRenderCompleteSemaphore();

		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_Swapchain->swapchain;
		info.pImageIndices = &m_Frames[s_FrameIndex].m_ImageIndex;
		VkResult err = vkQueuePresentKHR(m_Context->GetGraphicsQueueVK(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_RebuildSwapchain = true;
			return false;
		}

		if (!check_vk_result(err))
		{
			Log::Error("(renderer 1)");
		}

		s_PreviousFrame = s_FrameIndex;
		s_FrameIndex = (s_FrameIndex + 1) % m_Swapchain->imageCount;
		return true;
	}

	void VulkanRenderer::AddImguiPass()
	{
		m_IMGUIPass = std::make_shared<ImguiPass>();
		m_IMGUIPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		m_IMGUIPass->SetImguiState(m_Imgui);
	}

	void VulkanRenderer::CaptureCursor()
	{
		m_Window->GetWindow()->CaptureCursor();
		m_Imgui->SetMouseInputEnabled(false);
	}

	void VulkanRenderer::ReleaseCursor()
	{
		m_Window->GetWindow()->ReleaseCursor();
		m_Imgui->SetMouseInputEnabled(true);
	}

	bool VulkanRenderer::BeginFrame(VulkanFrame*& currentFrame)
	{
		VkDevice vkDevice = m_Context->GetDevice()->GetLogicalDevice();
		const VkSemaphore* imageAcquired = m_Frames[s_FrameIndex].GetImageAcquiredSemaphore();
		const VkSemaphore* render_complete_semaphore = m_Frames[s_FrameIndex].GetRenderCompleteSemaphore();

		uint32_t imageIndex = 0;
		VkResult err = vkAcquireNextImageKHR(vkDevice, m_Swapchain->GetVK(), UINT64_MAX, *imageAcquired, VK_NULL_HANDLE, &imageIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_RebuildSwapchain = true;
			return false;
		}

		if (!check_vk_result(err))
		{
			Log::Error("(renderer 2)");
		}

		VulkanFrame& frame = m_Frames[s_FrameIndex];
		frame.SetColorTexture(m_Swapchain->GetBackbuffers()[imageIndex], imageIndex);

		if (!check_vk_result(err))
		{
			Log::Error("(renderer 3)");
		}

		err = vkResetFences(vkDevice, 1, &frame.fence);

		if (!check_vk_result(err))
		{
			Log::Error("(renderer 4)");
		}

		// Allocate a command buffer
		Ref<VulkanCommandPool> commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());
		ResourceID commandBufferID = commandPool->AllocateBuffer();
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();

		// Transition the swapchain image back to a format for writing
		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(frame.GetFrameTexture());
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorTexture());
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		if (renderTarget->GetColorResolveTexture().IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorResolveTexture());
			commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		commandBuffer->EndCommands();
		commandBuffer->SubmitGraphics();
		commandPool->ReleaseBuffer(m_GraphicsCommandBuffers[s_FrameIndex]);

		currentFrame = &frame;
		return true;
	}

	void VulkanRenderer::RenderFrame()
	{
		VulkanFrame* frame = nullptr;
		if (BeginFrame(frame))
		{
			unsigned int width = m_Window->GetSurface()->GetWidth();
			unsigned int height = m_Window->GetSurface()->GetHeight();

			if (Scene* scene = SceneManager::GetActiveScene())
			{
				m_RenderScenes[s_FrameIndex]->ConvertScene(scene);
			}

			// RenderPass begin
			m_RenderingData->frame = frame;
			m_RenderingData->renderScene = m_RenderScenes[s_FrameIndex];
			m_RenderingData->width = width;
			m_RenderingData->height = height;

			RenderPassParams params;
			params.GraphicsCommandBuffer = m_GraphicsCommandBuffers[s_FrameIndex];
			params.context = m_Context;
			params.renderingData = m_RenderingData;
			params.FrameTexture = frame->GetFrameTexture();

			for (Ref<RenderPass>& renderPass : m_RenderPasses)
			{
				Ref<VulkanCommandPool> commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());
				ResourceID commandBufferID = commandPool->AllocateBuffer();
				Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

				params.GraphicsCommandBuffer = commandBufferID;
				commandBuffer->BeginCommands();
				renderPass->BeginPass(params);
				renderPass->Execute(params);
				renderPass->EndPass(params);

				commandBuffer->EndCommands();
				commandBuffer->SubmitGraphics();
				commandPool->ReleaseBuffer(commandBufferID);
			}

			// IMGUI always renders last
			if (m_IMGUIPass)
			{
				Ref<VulkanCommandPool> commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());
				ResourceID commandBufferID = commandPool->AllocateBuffer();
				Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);
				
				params.GraphicsCommandBuffer = commandBufferID;
				commandBuffer->BeginCommands();
				m_IMGUIPass->BeginPass(params);
				m_IMGUIPass->Execute(params);
				m_IMGUIPass->EndPass(params);

				commandBuffer->EndCommands();
				commandBuffer->SubmitGraphics();
				commandPool->ReleaseBuffer(commandBufferID);
			}

			Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(frame->GetFrameTexture());

			// Allocate a command buffer
			Ref<VulkanCommandPool> commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());
			ResourceID commandBufferID = commandPool->AllocateBuffer();
			Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

			commandBuffer->BeginCommands();

			if (renderTarget->GetColorResolveTexture().IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorResolveTexture());
				commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
			else
			{
				Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorTexture());
				commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

			// Wait for the initial fences to clear
			if (s_PreviousFrame != s_FrameIndex)
			{
				VkResult err = vkWaitForFences(m_Context->GetDeviceVK(), 1, &(m_Frames[s_PreviousFrame].fence), VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
				if (!check_vk_result(err))
				{
					Log::Error("(graphnode 0)");
				}
				ResourceManager::Flush();
			}

			VkResult err = vkQueueSubmit(m_Context->GetGraphicsQueueVK(), 1, &submitInfo, frame->fence);
			if (!check_vk_result(err))
			{
				Log::Error("(graphnode 1)");
			}
		}
	}

	void VulkanRenderer::RebuildSwapchain()
	{
		VulkanDevice* device = m_Context->GetDevice();
		device->WaitForIdle();

		m_Window->Resize(m_Swapchain.get());

		// Remake the swapchain
		m_Swapchain->Destroy();
		m_Swapchain = std::make_unique<VulkanSwapchain>(m_Context, m_Window->GetSurface());
		m_RebuildSwapchain = false;

		// Destroy the existing frames
		for (auto& frame : m_Frames)
		{
			frame.Destroy();
		}
		m_Frames.clear();

		// Remake the frame data with the new size
		SetupFrameData();
	}

	VulkanImgui::InitInfo VulkanRenderer::CreateImguiInitInfo()
	{
		VulkanImgui::InitInfo info;
		info.window = m_Window->GetWindow()->GetWindowHandle();
		info.instance = m_Context->GetInstance();
		info.physicalDevice = m_Context->GetPhysicalDevice()->GetPhysicalDevice();
		info.logicalDevice = m_Context->GetDevice()->GetLogicalDevice();
		info.queueIndex = m_Context->GetPhysicalDevice()->GetFamilyIndex(VulkanQueueType::Graphics);
		info.queue = m_Context->GetGraphicsQueueVK();
		info.descriptorPool = nullptr;
		info.renderPass = nullptr;
		info.minImageCount = m_Swapchain->minImageCount;
		info.imageCount = m_Swapchain->imageCount;
		info.colorFormat = m_Window->GetSurface()->GetFormat();
		return info;
	}

	void VulkanRenderer::SetupFrameData()
	{
		s_FrameIndex = 0;

		VkDevice vkDevice = m_Context->GetDevice()->GetLogicalDevice();
		VkFormat format = m_Window->GetSurface()->GetFormat();

		// Create the frames
		{
			std::vector<ResourceID> backbuffers = m_Swapchain->GetBackbuffers();
			uint32_t imageCount = m_Swapchain->GetImageCount();
			m_Frames.resize(imageCount);

			for (uint32_t i = 0; i < imageCount; ++i)
			{
				m_Frames[i] = VulkanFrame(m_Context, backbuffers[i], format);
			}
		}

	}
}