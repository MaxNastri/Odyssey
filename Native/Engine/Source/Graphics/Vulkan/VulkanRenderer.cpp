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
#include "VulkanDescriptorSet.h"
#include "ResourceManager.h"
#include "RenderGraphNodes.h"
#include "PerFrameRenderingData.h"

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		context = std::make_shared<VulkanContext>();
		ResourceManager::Initialize(context);
		context->SetupResources();

		window = std::make_shared<VulkanWindow>(context);
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		descriptorPool = std::make_unique<VulkanDescriptorPool>(context->GetDevice());

		// Shaders
		fragmentShader = ResourceManager::AllocateShader(ShaderType::Fragment, "frag.spv");
		vertexShader = ResourceManager::AllocateShader(ShaderType::Vertex, "vert.spv");

		// Pipeline
		VulkanPipelineInfo pipelineInfo;
		pipelineInfo.fragmentShader = fragmentShader;
		pipelineInfo.vertexShader = vertexShader;
		graphicsPipeline = ResourceManager::AllocateGraphicsPipeline(pipelineInfo);

		// IMGUI
		VulkanImgui::InitInfo imguiInfo = CreateImguiInitInfo();
		imgui = std::make_shared<VulkanImgui>(context, imguiInfo);

		SetupFrameData();
		for (int i = 0; i < frames.size(); ++i)
		{
			commandPools.push_back(ResourceManager::AllocateCommandPool());
			commandBuffers.push_back(commandPools[i].Get()->AllocateBuffer());
		}

		// Draw data
		renderTexture = ResourceManager::AllocateTexture(1000, 1000);
		rtSet = imgui->AddTexture(renderTexture.Get());
		InitDrawCalls();
		BuildRenderGraph();
	}

	void VulkanRenderer::Destroy()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		uboDescriptor->Destroy();

		for (auto uboBuffer : uboBuffers)
		{
			ResourceManager::DestroyBuffer(uboBuffer);
		}
		uboBuffers.clear();

		ResourceManager::DestroyTexture(renderTexture);

		for (auto vertexBuffer : m_VertexBuffers)
		{
			ResourceManager::DestroyVertexBuffer(vertexBuffer);
		}
		m_VertexBuffers.clear();

		for (auto indexBuffer : m_IndexBuffers)
		{
			ResourceManager::DestroyIndexBuffer(indexBuffer);
		}
		m_IndexBuffers.clear();

		for (int i = 0; i < frames.size(); ++i)
		{
			frames[i].Destroy();
		}

		frames.clear();
		ResourceManager::DestroyGraphicsPipeline(graphicsPipeline);
		descriptorPool.reset();
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
		}

		check_vk_result(err);
		frameIndex = (frameIndex + 1) % swapchain->imageCount;
		return true;
	}

	void VulkanRenderer::BuildRenderGraph()
	{
		renderingData = std::make_shared<PerFrameRenderingData>();
		BeginPassNode* beginSceneView = m_RenderGraph.CreateNode<BeginPassNode>("Begin Scene View", vertexShader, fragmentShader, renderTexture);
		DrawNode* drawSceneView = m_RenderGraph.CreateNode<DrawNode>("Draw Scene View");
		EndPassNode* endSceneView = m_RenderGraph.CreateNode<EndPassNode>("End Scene View", renderTexture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		BeginPassNode* beginImgui = m_RenderGraph.CreateNode<BeginPassNode>("Begin Imgui View", vertexShader, fragmentShader);
		ImguiDrawNode* drawImgui = m_RenderGraph.CreateNode<ImguiDrawNode>("Draw Imgui View", imgui);
		drawImgui->AddDescriptorSet(rtSet);
		EndPassNode* endImgui = m_RenderGraph.CreateNode<EndPassNode>("End Imgui View", VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		SubmitNode* submit = m_RenderGraph.CreateNode<SubmitNode>("Submit Frame");

		m_RenderGraph.SetRootNode(beginSceneView);
		beginSceneView->SetNext(drawSceneView);
		drawSceneView->SetNext(endSceneView);
		endSceneView->SetNext(beginImgui);
		beginImgui->SetNext(drawImgui);
		drawImgui->SetNext(endImgui);
		endImgui->SetNext(submit);
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

			// RenderPass begin
			VulkanCommandBuffer* commandBuffer = commandBuffers[frameIndex].Get();
			renderingData->frame = frame;
			renderingData->m_Drawcalls = m_DrawCalls;
			renderingData->width = width;
			renderingData->height = height;
			m_RenderGraph.Setup(context.get(), renderingData.get(), commandBuffers[frameIndex]);
			m_RenderGraph.Execute(context.get(), renderingData.get(), commandBuffers[frameIndex]);
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

	void VulkanRenderer::InitDrawCalls()
	{
		// Create the render object first
		{
			std::vector<VulkanVertex> vertices;
			vertices.resize(4);
			vertices[0] = VulkanVertex(glm::vec3(-0.5f, -0.5f, 0), glm::vec3(1, 0, 0));
			vertices[1] = VulkanVertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0, 1, 0));
			vertices[2] = VulkanVertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0, 0, 1));
			vertices[3] = VulkanVertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1));

			std::vector<uint32_t> indices{ 0, 1, 2, 2, 3, 0 };

			ResourceHandle<Mesh> mesh = ResourceManager::AllocateMesh(vertices, indices);
			m_RenderObjects.clear();
			m_RenderObjects.push_back(RenderObject(mesh));
		}

		// Convert the render objects into draw calls
		{
			m_DrawCalls.resize(m_RenderObjects.size());

			for (auto& renderObject : m_RenderObjects)
			{
				m_DrawCalls[0].SetMesh(renderObject.m_Mesh);
			}
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
		info.queue = context->GetGraphicsQueueVK();
		info.descriptorPool = descriptorPool->descriptorPool;
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

		// Create the ubos per frame
		{
			uboBuffers.resize(frames.size());
			uboData.resize(frames.size());

			// Create the UBO descriptor
			uboDescriptor = std::make_shared<VulkanDescriptorSet>(context);

			for (int i = 0; i < frames.size(); ++i)
			{
				uboData[i].world = glm::identity<glm::mat4x4>();
				uboData[i].inverseView = glm::identity<glm::mat4x4>();
				uboData[i].proj = glm::identity<glm::mat4x4>();

				uint32_t bufferSize = sizeof(uboData[i]);
				uboBuffers[i] = ResourceManager::AllocateBuffer(BufferType::Uniform, bufferSize);
				uboBuffers[i].Get()->AllocateMemory();
				uboBuffers[i].Get()->SetMemory(bufferSize, &uboData[i]);
			}
		}
	}
}