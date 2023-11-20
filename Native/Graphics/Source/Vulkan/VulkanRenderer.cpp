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

namespace Odyssey
{
	VulkanRenderer::VulkanRenderer()
	{
		context = std::make_shared<VulkanContext>();
		context->SetupResources();
		window = std::make_shared<VulkanWindow>(context);
		swapchain = std::make_unique<VulkanSwapchain>(context, window->GetSurface());
		descriptorPool = std::make_unique<VulkanDescriptorPool>(context->GetDevice());

		// Shaders
		fragmentShader = std::make_unique<VulkanShader>(context, ShaderType::Fragment, "frag.spv");
		vertexShader = std::make_unique<VulkanShader>(context, ShaderType::Vertex, "vert.spv");

		// Pipeline
		VulkanPipelineInfo pipelineInfo;
		pipelineInfo.fragmentShader = fragmentShader.get();
		pipelineInfo.vertexShader = vertexShader.get();
		graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(context, pipelineInfo);

		// IMGUI
		VulkanImgui::InitInfo imguiInfo = CreateImguiInitInfo();
		imgui = std::make_unique<VulkanImgui>(context, imguiInfo);

		SetupFrameData();
		for (int i = 0; i < frames.size(); ++i)
		{
			commandPool.push_back(std::make_unique<VulkanCommandPool>(context));
			commandBuffers.push_back(commandPool[i]->AllocateBuffer());
		}

		// Draw data
		InitDrawCalls();
		renderTexture = std::make_shared<VulkanTexture>(context, 1000, 1000);
		rtSet = imgui->AddTexture(renderTexture.get());
	}

	void VulkanRenderer::Destroy()
	{
		VulkanDevice* device = context->GetDevice();
		device->WaitForIdle();

		for (auto vertexBuffer : m_VertexBuffers)
		{
			vertexBuffer->Destroy();
		}
		m_VertexBuffers.clear();

		for (auto indexBuffer : m_IndexBuffers)
		{
			indexBuffer->Destroy();
		}
		m_IndexBuffers.clear();

		for (int i = 0; i < frames.size(); ++i)
		{
			frames[i].Destroy();
		}

		frames.clear();

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

		commandPool[frameIndex]->Reset();

		// Command buffer begin
		commandBuffers[frameIndex]->BeginCommands();

		// Transition the swapchain image back to a format for writing
		commandBuffers[frameIndex]->TransitionLayouts(frame.GetRenderTarget(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

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
			unsigned int width = window->GetSurface()->GetWidth();
			unsigned int height = window->GetSurface()->GetHeight();

			// RenderPass begin
			VulkanCommandBuffer* commandBuffer = commandBuffers[frameIndex];

			VkClearValue ClearValue;
			ClearValue.color.float32[0] = 0.0f;
			ClearValue.color.float32[1] = 0.0f;
			ClearValue.color.float32[2] = 0.0f;
			ClearValue.color.float32[3] = 0.0f;

			// Render to a texture the opaques
			{
				VkRenderingAttachmentInfoKHR color_attachment_info{};
				color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
				color_attachment_info.pNext = VK_NULL_HANDLE;
				color_attachment_info.imageView = renderTexture->GetImage()->GetImageView();
				color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
				color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				color_attachment_info.clearValue = ClearValue;

				VkRenderingInfoKHR rendering_info = {};
				rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
				rendering_info.pNext = VK_NULL_HANDLE;
				rendering_info.flags = 0;
				rendering_info.renderArea = VkRect2D{ VkOffset2D{}, VkExtent2D{1000, 1000} };
				rendering_info.layerCount = 1;
				rendering_info.viewMask = 0;
				rendering_info.colorAttachmentCount = 1;
				rendering_info.pColorAttachments = &color_attachment_info;
				rendering_info.pDepthAttachment = VK_NULL_HANDLE;
				rendering_info.pStencilAttachment = VK_NULL_HANDLE;

				commandBuffer->BeginRendering(rendering_info);
				commandBuffer->BindPipeline(graphicsPipeline.get());

				// Viewport
				{
					VkViewport viewport{};
					viewport.x = 0.0f;
					viewport.y = 0.0f;
					viewport.width = static_cast<float>(1000);
					viewport.height = static_cast<float>(1000);
					viewport.minDepth = 0.0f;
					viewport.maxDepth = 1.0f;
					commandBuffer->BindViewport(viewport);
				}

				// Scissor
				{
					VkRect2D scissor{};
					scissor.offset = { 0, 0 };
					scissor.extent = VkExtent2D{ 1000, 1000 };
					commandBuffer->SetScissor(scissor);
				}

				for (auto& drawCall : m_DrawCalls)
				{
					commandBuffer->BindVertexBuffer(drawCall.VertexBuffer.get());
					commandBuffer->BindIndexBuffer(drawCall.IndexBuffer.get());
					commandBuffer->DrawIndexed(drawCall.IndexCount, 1, 0, 0, 0);
				}
				commandBuffer->EndRendering();
				commandBuffer->TransitionLayouts(renderTexture->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			}

			// rendering GUI to the back buffer
			{
				VkRenderingAttachmentInfoKHR color_attachment_info{};
				color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
				color_attachment_info.pNext = VK_NULL_HANDLE;
				color_attachment_info.imageView = frame->GetRenderTargetViewVK();
				color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
				color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				color_attachment_info.clearValue = ClearValue;

				VkRenderingInfoKHR rendering_info = {};
				rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
				rendering_info.pNext = VK_NULL_HANDLE;
				rendering_info.flags = 0;
				rendering_info.renderArea = VkRect2D{ VkOffset2D{}, VkExtent2D{width, height} };
				rendering_info.layerCount = 1;
				rendering_info.viewMask = 0;
				rendering_info.colorAttachmentCount = 1;
				rendering_info.pColorAttachments = &color_attachment_info;
				rendering_info.pDepthAttachment = VK_NULL_HANDLE;
				rendering_info.pStencilAttachment = VK_NULL_HANDLE;

				commandBuffer->BeginRendering(rendering_info);
				commandBuffer->BindPipeline(graphicsPipeline.get());


				// Viewport
				{
					VkViewport viewport{};
					viewport.x = 0.0f;
					viewport.y = 0.0f;
					viewport.width = static_cast<float>(width);
					viewport.height = static_cast<float>(height);
					viewport.minDepth = 0.0f;
					viewport.maxDepth = 1.0f;
					commandBuffer->BindViewport(viewport);
				}

				// Scissor
				{
					VkRect2D scissor{};
					scissor.offset = { 0, 0 };
					scissor.extent = VkExtent2D{ width, height };
					commandBuffer->SetScissor(scissor);
				}

				// TODO: DRAW
				imgui->Render(commandBuffer->GetCommandBuffer(), rtSet);
				commandBuffer->EndRendering();

				// Transition the backbuffer layout for presenting
				commandBuffer->TransitionLayouts(frames[frameIndex].GetRenderTarget(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}

			commandBuffer->EndCommands();

			// TODO: Move this into the context?
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

			VkResult err = vkQueueSubmit(context->GetGraphicsQueueVK(), 1, &submitInfo, frame->fence);
			check_vk_result(err);
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

			m_RenderObjects.clear();
			m_RenderObjects.push_back(RenderObject(vertices, indices));
		}

		// Convert the render objects into draw calls
		{
			m_DrawCalls.resize(m_RenderObjects.size());

			for (auto& renderObject : m_RenderObjects)
			{
				size_t vertexDataSize = renderObject.Vertices.size() * sizeof(VulkanVertex);
				size_t indexDataSize = renderObject.Indices.size() * sizeof(uint32_t);

				// Create and assign vertex buffer
				m_VertexBuffers.push_back(std::make_shared<VulkanVertexBuffer>(context, renderObject.Vertices));
				m_DrawCalls[0].VertexBuffer = m_VertexBuffers[m_VertexBuffers.size() - 1];

				// Create and assign index buffer
				m_IndexBuffers.push_back(std::make_shared<VulkanIndexBuffer>(context, renderObject.Indices));
				m_DrawCalls[0].IndexBuffer = m_IndexBuffers[m_IndexBuffers.size() - 1];

				m_DrawCalls[0].IndexCount = (uint32_t)renderObject.Indices.size();
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

		std::vector<std::shared_ptr<VulkanImage>> backbuffers = swapchain->GetBackbuffers();
		uint32_t imageCount = swapchain->GetImageCount();
		frames.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			frames[i] = VulkanFrame(context, backbuffers[i], format);
		}
	}

	VkRenderingInfo VulkanRenderer::GetRenderingInfo(VulkanFrame* frame)
	{
		VkClearValue ClearValue;
		ClearValue.color.float32[0] = 0.0f;
		ClearValue.color.float32[1] = 0.0f;
		ClearValue.color.float32[2] = 0.0f;
		ClearValue.color.float32[3] = 0.0f;

		unsigned int width = window->GetSurface()->GetWidth();
		unsigned int height = window->GetSurface()->GetHeight();

		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = frame->GetRenderTargetViewVK();
		color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
		color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_info.clearValue = ClearValue;

		VkRenderingInfoKHR rendering_info = {};
		rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		rendering_info.pNext = VK_NULL_HANDLE;
		rendering_info.flags = 0;
		rendering_info.renderArea = VkRect2D{ VkOffset2D{}, VkExtent2D{width, height} };
		rendering_info.layerCount = 1;
		rendering_info.viewMask = 0;
		rendering_info.colorAttachmentCount = 1;
		rendering_info.pColorAttachments = &color_attachment_info;
		rendering_info.pDepthAttachment = VK_NULL_HANDLE;
		rendering_info.pStencilAttachment = VK_NULL_HANDLE;

		return rendering_info;
	}
}