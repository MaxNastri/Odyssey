#include "RenderPasses.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"
#include "Logger.h"
#include "PerFrameRenderingData.h"
#include "Drawcall.h"
#include "RenderScene.h"
#include "VulkanImgui.h"
#include "VulkanDescriptorBuffer.h"

namespace Odyssey
{
	void RenderPass::SetRenderTarget(ResourceHandle<VulkanTexture> renderTarget)
	{
		m_RenderTexture = renderTarget;
		m_RenderTarget = renderTarget.Get()->GetImage();
	}

	void RenderPass::SetRenderTarget(VulkanImage* renderTarget)
	{
		m_RenderTarget = renderTarget;
	}

	OpaquePass::OpaquePass()
	{
		m_ClearValue = glm::vec4(0, 0, 0, 1);
	}

	void OpaquePass::BeginPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		VulkanImage* renderTarget = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;

		// Extract the render target and width/height
		if (m_RenderTexture.IsValid())
		{
			VulkanTexture* renderTexture = m_RenderTexture.Get();
			renderTarget = renderTexture->GetImage();
			width = renderTexture->GetWidth();
			height = renderTexture->GetHeight();
		}
		else
		{
			Logger::LogError("(OpaquePass) Invalid render target for opaque pass.");
			return;
		}

		// Transfer the clear value into vulkan type
		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		// Create the rendering attachment for the render target
		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = renderTarget->GetImageView();
		color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
		color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_info.clearValue = clearValue;

		// Rendering info
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

		// Begin dynamic rendering
		commandBuffer->BeginRendering(rendering_info);

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
	}

	void OpaquePass::Execute(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();
		std::shared_ptr<RenderScene> renderScene = params.renderingData->renderScene;

		// Bind the scene descriptor buffer
		commandBuffer->BindDescriptorBuffer(renderScene->descriptorBuffer);

		for (auto& setPass : params.renderingData->renderScene->setPasses)
		{
			commandBuffer->BindPipeline(setPass.pipeline);

			// Set the scene descriptor buffer offset
			uint32_t buffer_index_ubo = 0;
			VkDeviceSize buffer_offset = 0;

			commandBuffer->SetDescriptorBufferOffset(setPass.pipeline, 0, &buffer_index_ubo, &buffer_offset);

			uint32_t descriptorIndex = 1;

			for (auto& drawcall : setPass.drawcalls)
			{
				// Bind the per-objectdescriptor buffer 
				buffer_offset = descriptorIndex * renderScene->descriptorBuffer.Get()->GetSize();
				commandBuffer->SetDescriptorBufferOffset(setPass.pipeline, 1, &buffer_index_ubo, &buffer_offset);
				descriptorIndex++;

				// Set the per-object descriptor buffer offset
				commandBuffer->BindVertexBuffer(drawcall.VertexBuffer);
				commandBuffer->BindIndexBuffer(drawcall.IndexBuffer);
				commandBuffer->DrawIndexed(drawcall.IndexCount, 1, 0, 0, 0);
			}
		}
	}

	void OpaquePass::EndPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Make sure the RT is valid
		if (!m_RenderTexture.IsValid())
		{
			Logger::LogError("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		VulkanImage* renderTarget = m_RenderTexture.Get()->GetImage();
		commandBuffer->TransitionLayouts(renderTarget, m_OldLayout, m_NewLayout);
	}

	void ImguiPass::SetRenderTarget(VulkanImage* renderTarget)
	{
		m_RenderTarget = renderTarget;
	}

	void ImguiPass::BeginPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		uint32_t width = params.renderingData->width;
		uint32_t height = params.renderingData->height;

		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = m_RenderTarget->GetImageView();
		color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
		color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_info.clearValue = clearValue;

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
	}

	void ImguiPass::Execute(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();
		m_Imgui->Render(commandBuffer->GetCommandBuffer());
	}

	void ImguiPass::EndPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Transition the backbuffer layout for presenting
		commandBuffer->TransitionLayouts(m_RenderTarget, m_OldLayout, m_NewLayout);
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}
}