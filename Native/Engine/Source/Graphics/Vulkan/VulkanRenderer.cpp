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
#include "RenderGraphNodes.h"
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

		// Drawing
		descriptorLayouts.push_back(ResourceManager::AllocateDescriptorLayout(DescriptorType::Uniform, ShaderStage::Vertex, 0));
		SetupFrameData();
		SetupDrawData();

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

		for (auto uboBuffer : uboBuffers)
		{
			ResourceManager::DestroyBuffer(uboBuffer);
		}
		uboBuffers.clear();

		ResourceManager::DestroyTexture(renderTexture);

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

		if (!m_RenderGraphCreated)
		{
			BuildRenderGraph();
			m_RenderGraphCreated = true;
		}
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

		if (!check_vk_result(err))
		{
			Logger::LogError("(renderer 1)");
		}
		frameIndex = (frameIndex + 1) % swapchain->imageCount;
		return true;
	}

	void VulkanRenderer::BuildRenderGraph()
	{
		// Convert the scene into a render scene
		Scene* scene = SceneManager::GetActiveScene();
		RenderScene renderScene(scene);

		renderingData = std::make_shared<PerFrameRenderingData>();

		// Begin pass
		BeginPassNode* beginSceneView = m_RenderGraph.CreateNode<BeginPassNode>("Begin Scene View", renderTexture);
		RenderGraphNode* currentNode = beginSceneView;

		// Foreach object, make a set pipeline + draw
		for (RenderObject& renderObject : renderScene.m_RenderObjects)
		{
			Material* material = renderObject.Material.Get();
			SetPipelineNode* pipelineNode = m_RenderGraph.CreateNode<SetPipelineNode>("Set Pipeline", material->GetVertexShader(), material->GetFragmentShader(), descriptorLayouts);
			currentNode->SetNext(pipelineNode);
			currentNode = pipelineNode;

			Drawcall drawcall;
			drawcall.SetMesh(renderObject.Mesh);
			m_DrawCalls.push_back(drawcall);
			DrawNode* drawNode = m_RenderGraph.CreateNode<DrawNode>("Draw Scene View", drawcall);
			currentNode->SetNext(drawNode);
			currentNode = drawNode;
		}

		EndPassNode* endSceneView = m_RenderGraph.CreateNode<EndPassNode>("End Scene View", renderTexture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		currentNode->SetNext(endSceneView);
		currentNode = endSceneView;

		BeginPassNode* beginImgui = m_RenderGraph.CreateNode<BeginPassNode>("Begin Imgui View");
		currentNode->SetNext(beginImgui);
		currentNode = beginImgui;

		ImguiDrawNode* drawImgui = m_RenderGraph.CreateNode<ImguiDrawNode>("Draw Imgui View", imgui);
		drawImgui->AddDescriptorSet(rtSet);
		currentNode->SetNext(drawImgui);
		currentNode = drawImgui;

		EndPassNode* endImgui = m_RenderGraph.CreateNode<EndPassNode>("End Imgui View", VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		currentNode->SetNext(endImgui);
		currentNode = endImgui;

		SubmitNode* submit = m_RenderGraph.CreateNode<SubmitNode>("Submit Frame");
		currentNode->SetNext(submit);
		currentNode = submit;

		m_RenderGraph.SetRootNode(beginSceneView);
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
			renderingData->descriptorBuffer = sceneBuffer[frameIndex];
			renderingData->uniformBuffer = uboBuffers[frameIndex];


			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			uboData[frameIndex].world = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			uint32_t bufferSize = sizeof(uboData[frameIndex]);
			uboBuffers[frameIndex].Get()->SetMemory(bufferSize, &uboData[frameIndex]);

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

	void VulkanRenderer::SetupDrawData()
	{
		// Draw data
		renderTexture = ResourceManager::AllocateTexture(1000, 1000);
		rtSet = imgui->AddTexture(renderTexture.Get());

		// Create the ubos per frame
		{
			uboBuffers.resize(frames.size());
			uboData.resize(frames.size());

			for (int i = 0; i < frames.size(); ++i)
			{
				uboData[i].world = glm::identity<mat4>();// glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				uboData[i].inverseView = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				uboData[i].proj = glm::perspectiveLH(glm::radians(45.0f), 1000.0f / 1000.0f, 0.1f, 10.0f);
				uboData[i].proj[1][1] = -uboData[i].proj[1][1];

				uint32_t bufferSize = sizeof(uboData[i]);
				uboBuffers[i] = ResourceManager::AllocateBuffer(BufferType::Uniform, bufferSize);
				uboBuffers[i].Get()->AllocateMemory();
				uboBuffers[i].Get()->SetMemory(bufferSize, &uboData[i]);
			}
		}

		sceneBuffer.resize(frames.size());

		for (int i = 0; i < frames.size(); i++)
		{
			sceneBuffer[i] = ResourceManager::AllocateDescriptorBuffer(descriptorLayouts[0], 1);
			sceneBuffer[i].Get()->SetUniformBuffer(uboBuffers[i], 0);
		}
	}
}