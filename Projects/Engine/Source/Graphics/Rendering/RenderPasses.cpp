#include "RenderPasses.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderTexture.h"
#include "Log.h"
#include "PerFrameRenderingData.h"
#include "Drawcall.h"
#include "RenderScene.h"
#include "VulkanImgui.h"
#include "VulkanPushDescriptors.h"
#include "VulkanTexture.h"
#include "DebugRenderer.h"
#include "Shader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanContext.h"
#include "RenderSubPasses.h"

namespace Odyssey
{
	ShadowPass::ShadowPass()
	{
		VulkanImageDescription imageDesc;
		imageDesc.Width = Shadowmap_Size;
		imageDesc.Height = Shadowmap_Size;
		imageDesc.Format = TextureFormat::D24_UNORM_S8_UINT;
		imageDesc.ImageType = ImageType::Shadowmap;
		imageDesc.Samples = 1;

		m_DepthRT = ResourceManager::Allocate<VulkanRenderTexture>(imageDesc);
		m_SubPasses.push_back(std::make_shared<ShadowSubPass>());

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void ShadowPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		uint32_t width = 0;
		uint32_t height = 0;

		// Transfer the clear value into vulkan type
		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		// Create the rendering attachment for the render target
		std::vector<VkRenderingAttachmentInfoKHR> attachments;

		if (m_DepthRT.IsValid())
		{
			VkClearValue clearValue;
			clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			auto depthTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_DepthRT);
			ResourceID depthAttachmentID = depthTexture->GetImage();
			width = depthTexture->GetWidth();
			height = depthTexture->GetHeight();

			// Load the image
			auto depthAttachment = ResourceManager::GetResource<VulkanImage>(depthAttachmentID);

			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthAttachment->GetImageView();
			depthAttachmentInfo.imageLayout = depthAttachment->GetLayout();
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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
		rendering_info.colorAttachmentCount = 0;
		rendering_info.pDepthAttachment = &attachments[0];

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

	void ShadowPass::Execute(RenderPassParams& params)
	{
		RenderSubPassData subPassData;

		for (auto& renderSubPass : m_SubPasses)
		{
			renderSubPass->Execute(params, subPassData);
		}
	}

	void ShadowPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Make sure the RT is valid
		if (!m_DepthRT.IsValid())
		{
			Log::Error("[ShadowPass] Invalid render target for Shadow Pass");
			return;
		}

		// Transition the shadowmap for use in the fragment shader
		auto depthTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_DepthRT);
		commandBuffer->TransitionLayouts(depthTexture->GetImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
	}

	OpaquePass::OpaquePass()
	{
		m_ClearValue = glm::vec4(0, 0, 0, 1);
		m_SubPasses.push_back(std::make_shared<SkyboxSubPass>());
		m_SubPasses.push_back(std::make_shared<OpaqueSubPass>());
		m_SubPasses.push_back(std::make_shared<ParticleSubPass>());

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void OpaquePass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		ResourceID colorAttachmentImage;
		uint32_t width = 0;
		uint32_t height = 0;

		// Extract the render target and width/height
		if (m_ColorRT.IsValid())
		{
			auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_ColorRT);
			colorAttachmentImage = renderTexture->GetImage();
			width = renderTexture->GetWidth();
			height = renderTexture->GetHeight();

			commandBuffer->TransitionLayouts(colorAttachmentImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			// Check if msaa is enabled
			if (params.context->GetSampleCount() > 1)
			{
				commandBuffer->TransitionLayouts(renderTexture->GetResolveImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
		}
		else
		{
			Log::Error("(OpaquePass) Invalid render target for opaque pass.");
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
			color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;

			if (params.context->GetSampleCount() > 1)
			{
				auto colorTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_ColorRT);
				auto resolveImage = ResourceManager::GetResource<VulkanImage>(colorTexture->GetResolveImage());

				color_attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
				color_attachment_info.resolveImageView = resolveImage->GetImageView();
				color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_attachment_info.clearValue = clearValue;

			attachments.push_back(color_attachment_info);
		}

		if (m_DepthRT.IsValid())
		{
			VkClearValue clearValue;
			clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			auto depthTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_DepthRT);
			ResourceID depthAttachmentID = depthTexture->GetImage();

			// Load the image
			auto depthAttachment = ResourceManager::GetResource<VulkanImage>(depthAttachmentID);

			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthAttachment->GetImageView();
			depthAttachmentInfo.imageLayout = depthAttachment->GetLayout();
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;

			if (params.context->GetSampleCount() > 1)
			{
				auto resolveImage = ResourceManager::GetResource<VulkanImage>(depthTexture->GetResolveImage());
				depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_MIN_BIT;
				depthAttachmentInfo.resolveImageView = resolveImage->GetImageView();
				depthAttachmentInfo.resolveImageLayout = resolveImage->GetLayout();
			}

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
		std::shared_ptr<RenderScene> renderScene = params.renderingData->renderScene;

		RenderSubPassData subPassData;
		subPassData.CameraIndex = RenderScene::MAX_CAMERAS;

		// Set either the camera override or the scene's main camera
		if (m_Camera)
		{
			subPassData.CameraIndex = renderScene->SetCameraData(m_Camera);
			subPassData.Camera = m_Camera;
		}
		else if (renderScene->HasMainCamera())
		{
			subPassData.CameraIndex = renderScene->SetCameraData(renderScene->m_MainCamera);
			subPassData.Camera = renderScene->m_MainCamera;
		}

		// Check for a valid camera data index
		if (subPassData.CameraIndex < RenderScene::MAX_CAMERAS)
		{
			for (auto& renderSubPass : m_SubPasses)
			{
				renderSubPass->Execute(params, subPassData);
			}
		}
	}

	void OpaquePass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Make sure the RT is valid
		if (!m_ColorRT.IsValid())
		{
			Log::Error("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		auto colorTexture = ResourceManager::GetResource<VulkanRenderTexture>(m_ColorRT);
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		if (params.context->GetSampleCount() > 1)
		{
			commandBuffer->TransitionLayouts(colorTexture->GetResolveImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	void OpaquePass::AddDebugSubPass()
	{
		std::shared_ptr<DebugSubPass> debugSubPass = std::make_shared<DebugSubPass>();
		debugSubPass->Setup();
		m_SubPasses.push_back(debugSubPass);
	}

	void ImguiPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		uint32_t width = params.renderingData->width;
		uint32_t height = params.renderingData->height;

		// If we don't have a valid RT set, use the back buffer
		Ref<VulkanRenderTexture> colorTexture = ResourceManager::GetResource<VulkanRenderTexture>(params.FrameTexture);
		Ref<VulkanImage> colorImage = ResourceManager::GetResource<VulkanImage>(colorTexture->GetImage());

		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = colorImage->GetImageView();
		color_attachment_info.imageLayout = colorImage->GetLayout();;
		color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;

		if (params.context->GetSampleCount() > 1)
		{
			Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(colorTexture->GetResolveImage());
			color_attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			color_attachment_info.resolveImageView = resolveImage->GetImageView();
			color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

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
		m_Imgui->Render(params.GraphicsCommandBuffer);
	}

	void ImguiPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		// Transition the backbuffer layout for presenting
		auto renderTexture = ResourceManager::GetResource<VulkanRenderTexture>(params.FrameTexture);
		commandBuffer->TransitionLayouts(renderTexture->GetImage(), m_EndLayout);

		if (params.context->GetSampleCount() > 1)
		{
			commandBuffer->TransitionLayouts(renderTexture->GetResolveImage(), m_EndLayout);
		}
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}

	void RenderPass::SetColorRenderTexture(ResourceID colorRT)
	{
		m_ColorRT = colorRT;
	}
	void RenderPass::SetDepthRenderTexture(ResourceID depthRT)
	{
		m_DepthRT = depthRT;
	}
}