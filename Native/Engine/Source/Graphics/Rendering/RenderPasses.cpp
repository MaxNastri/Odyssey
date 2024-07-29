#include "RenderPasses.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderTexture.h"
#include "Logger.h"
#include "PerFrameRenderingData.h"
#include "Drawcall.h"
#include "RenderScene.h"
#include "VulkanImgui.h"
#include "VulkanPushDescriptors.h"
#include "VulkanTexture.h"

namespace Odyssey
{
	void RenderPass::SetColorRenderTexture(ResourceHandle<VulkanRenderTexture> colorRT)
	{
		m_ColorRT = colorRT;
	}

	void RenderPass::SetDepthRenderTexture(ResourceHandle<VulkanRenderTexture> depthRT)
	{
		m_DepthRT = depthRT;
	}

	OpaquePass::OpaquePass()
	{
		m_ClearValue = glm::vec4(0, 0, 0, 1);
		pushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void OpaquePass::BeginPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		VulkanImage* colorAttachment = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;

		// Extract the render target and width/height
		if (VulkanRenderTexture* renderTexture = m_ColorRT.Get())
		{
			colorAttachment = renderTexture->GetImage().Get();
			width = renderTexture->GetWidth();
			height = renderTexture->GetHeight();

			commandBuffer->TransitionLayouts(renderTexture->GetImage(), m_BeginLayout);
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
		std::vector<VkRenderingAttachmentInfoKHR> attachments;

		{
			VkRenderingAttachmentInfoKHR color_attachment_info{};
			color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			color_attachment_info.pNext = VK_NULL_HANDLE;
			color_attachment_info.imageView = colorAttachment->GetImageView();
			color_attachment_info.imageLayout = colorAttachment->GetLayout();
			color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
			color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_attachment_info.clearValue = clearValue;

			attachments.push_back(color_attachment_info);
		}

		if (m_DepthRT.IsValid())
		{
			VkClearValue clearValue;
			clearValue.depthStencil = { 1.0f, 0 };

			VulkanImage* depthAttachment = m_DepthRT.Get()->GetImage().Get();
			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthAttachment->GetImageView();
			depthAttachmentInfo.imageLayout = depthAttachment->GetLayout();
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachmentInfo.clearValue = clearValue;
			attachments.push_back(depthAttachmentInfo);
		}

		// Rendering info
		VkRenderingInfoKHR rendering_info = {};
		rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		rendering_info.pNext = VK_NULL_HANDLE;
		rendering_info.flags = 0;
		rendering_info.renderArea = VkRect2D{ VkOffset2D{}, VkExtent2D{width, height} };
		rendering_info.layerCount = 1;
		rendering_info.viewMask = 0;
		rendering_info.colorAttachmentCount = 1;
		rendering_info.pColorAttachments = &attachments[0];
		rendering_info.pDepthAttachment = attachments.size() > 1 ? &attachments[1] : VK_NULL_HANDLE;
		rendering_info.pStencilAttachment = attachments.size() > 1 ? &attachments[1] : VK_NULL_HANDLE;

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

		if (m_Camera)
			renderScene->SetCameraData(m_Camera);

		if (m_Camera || renderScene->HasMainCamera())
		{
			for (auto& setPass : params.renderingData->renderScene->setPasses)
			{
				commandBuffer->BindPipeline(setPass.pipeline);

				for (size_t i = 0; i < setPass.drawcalls.size(); i++)
				{
					// Prepare the push descriptor commands
					pushDescriptors->Clear();
					pushDescriptors->Add(renderScene->sceneUniformBuffer, 0);
					pushDescriptors->Add(renderScene->perObjectUniformBuffers[i], 1);
					if (setPass.Texture.IsValid())
					{
						pushDescriptors->Add(setPass.Texture, 2);
					}

					// Push the descriptors into the command buffer
					commandBuffer->PushDescriptors(pushDescriptors.get(), setPass.pipeline);

					// Set the per-object descriptor buffer offset
					Drawcall& drawcall = setPass.drawcalls[i];
					commandBuffer->BindVertexBuffer(drawcall.VertexBuffer);
					commandBuffer->BindIndexBuffer(drawcall.IndexBuffer);
					commandBuffer->DrawIndexed(drawcall.IndexCount, 1, 0, 0, 0);
				}
			}
		}
	}

	void OpaquePass::EndPass(RenderPassParams& params)
	{
		VulkanCommandBuffer* commandBuffer = params.commandBuffer.Get();

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Make sure the RT is valid
		if (!m_ColorRT.IsValid())
		{
			Logger::LogError("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		commandBuffer->TransitionLayouts(m_ColorRT, m_EndLayout);
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
		color_attachment_info.imageView = m_ColorRT.Get()->GetImage().Get()->GetImageView();
		color_attachment_info.imageLayout = m_ColorRT.Get()->GetImage().Get()->GetLayout();;
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
		commandBuffer->TransitionLayouts(m_ColorRT, m_EndLayout);
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}
}