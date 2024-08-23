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
	OpaquePass::OpaquePass()
	{
		m_ClearValue = glm::vec4(0, 0, 0, 1);
		pushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void OpaquePass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.commandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		ResourceID colorAttachmentImage;
		uint32_t width = 0;
		uint32_t height = 0;

		// Extract the render target and width/height
		if (m_ColorRT)
		{
			auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_ColorRT);
			colorAttachmentImage = renderTexture->GetImage();
			width = renderTexture->GetWidth();
			height = renderTexture->GetHeight();

			commandBuffer->TransitionLayouts(colorAttachmentImage, m_BeginLayout);
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
			auto colorAttachment = ResourceManager::GetResource<VulkanImage>(colorAttachmentImage);
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

		if (m_DepthRT)
		{
			VkClearValue clearValue;
			clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_DepthRT);
			ResourceID depthAttachmentID = renderTexture->GetImage();

			// Load the image
			auto depthAttachment = ResourceManager::GetResource<VulkanImage>(depthAttachmentID);

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
		ResourceID commandBufferID = params.commandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		std::shared_ptr<RenderScene> renderScene = params.renderingData->renderScene;

		uint32_t cameraIndex = RenderScene::MAX_CAMERAS;

		// Set either the camera override or the scene's main camera
		if (m_Camera)
			cameraIndex = renderScene->SetCameraData(m_Camera);
		else if (renderScene->HasMainCamera())
			cameraIndex = renderScene->SetCameraData(renderScene->m_MainCamera);

		// Check for a valid camera data index
		if (cameraIndex < RenderScene::MAX_CAMERAS)
		{
			for (auto& setPass : params.renderingData->renderScene->setPasses)
			{
				commandBuffer->BindPipeline(setPass.GraphicsPipeline);

				for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
				{
					// Add the camera and per object data to the push descriptors
					pushDescriptors->Clear();
					pushDescriptors->AddBuffer(renderScene->cameraDataBuffers[cameraIndex], 0);
					pushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[i], 1);

					// Add textures, if they are set
					if (setPass.Texture)
					{
						pushDescriptors->AddTexture(setPass.Texture, 2);
					}

					// Push the descriptors into the command buffer
					commandBuffer->PushDescriptors(pushDescriptors.get(), setPass.GraphicsPipeline);

					// Set the per-object descriptor buffer offset
					Drawcall& drawcall = setPass.Drawcalls[i];
					commandBuffer->BindVertexBuffer(drawcall.VertexBufferID);
					commandBuffer->BindIndexBuffer(drawcall.IndexBufferID);
					commandBuffer->DrawIndexed(drawcall.IndexCount, 1, 0, 0, 0);
				}
			}
		}
	}

	void OpaquePass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.commandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Make sure the RT is valid
		if (!m_ColorRT)
		{
			Logger::LogError("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_ColorRT);
		commandBuffer->TransitionLayouts(renderTexture->GetImage(), m_EndLayout);
	}

	void ImguiPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.commandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		uint32_t width = params.renderingData->width;
		uint32_t height = params.renderingData->height;

		// If we don't have a valid RT set, use the back buffer
		ResourceID renderTargetID = m_ColorRT.IsValid() ? m_ColorRT : params.FrameRT;
		auto renderTarget = ResourceManager::GetResource<VulkanRenderTexture>(renderTargetID);
		auto renderTargetImage = ResourceManager::GetResource<VulkanImage>(renderTarget->GetImage());

		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = renderTargetImage->GetImageView();
		color_attachment_info.imageLayout = renderTargetImage->GetLayout();;
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
		m_Imgui->Render(params.commandBuffer);
	}

	void ImguiPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.commandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		// If we don't have a valid RT set, use the back buffer
		ResourceID renderTarget = m_ColorRT.IsValid() ? m_ColorRT : params.FrameRT;
		
		// Transition the backbuffer layout for presenting
		auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(renderTarget);
		commandBuffer->TransitionLayouts(renderTexture->GetImage(), m_EndLayout);
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}
}