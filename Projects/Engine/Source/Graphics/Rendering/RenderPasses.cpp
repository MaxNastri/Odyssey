#include "RenderPasses.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
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

#include "RenderTarget.h"
#include "Renderer.h"

namespace Odyssey
{
	DepthPass::DepthPass(uint8_t cameraTag)
	{
		m_Camera = cameraTag;

		VulkanImageDescription imageDesc;
		imageDesc.Width = Texture_Size;
		imageDesc.Height = Texture_Size;
		imageDesc.Format = TextureFormat::D32_SFLOAT;
		imageDesc.ImageType = ImageType::Shadowmap;
		imageDesc.Samples = 1;

		m_RenderTarget = ResourceManager::Allocate<RenderTarget>(imageDesc, RenderTargetFlags::Depth);
		m_SubPasses.push_back(std::make_shared<DepthSubPass>());

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void DepthPass::BeginPass(RenderPassParams& params)
	{
		if (params.renderingData->renderScene->SetPasses.size() == 0)
			return;

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

		if (Camera* camera = params.renderingData->renderScene->GetCamera(m_Camera))
		{
			if (camera->GetViewportWidth() != width || camera->GetViewportHeight() != height)
			{
				ResourceManager::Destroy(m_RenderTarget);

				VulkanImageDescription imageDesc;
				imageDesc.Width = width = camera->GetViewportWidth();
				imageDesc.Height = height = camera->GetViewportHeight();
				imageDesc.Format = TextureFormat::D32_SFLOAT;
				imageDesc.ImageType = ImageType::Shadowmap;
				imageDesc.Samples = 1;

				m_RenderTarget = ResourceManager::Allocate<RenderTarget>(imageDesc, RenderTargetFlags::Depth);
			}
		}

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID depthTextureID = renderTarget->GetDepthTexture();

		if (depthTextureID.IsValid())
		{
			VkClearValue clearValue;
			if (Renderer::ReverseDepthEnabled())
				clearValue.depthStencil = { 0.0f, 0 };
			else
				clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			Ref<VulkanTexture> depthTexture = ResourceManager::GetResource<VulkanTexture>(depthTextureID);

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
			viewport.y = (float)height;
			viewport.width = (float)width;
			viewport.height = -1.0f * (float)height;
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

	void DepthPass::Execute(RenderPassParams& params)
	{
		if (params.renderingData->renderScene->SetPasses.size() == 0)
			return;

		RenderSubPassData subPassData;
		subPassData.CameraTag = m_Camera;

		for (auto& renderSubPass : m_SubPasses)
		{
			renderSubPass->Execute(params, subPassData);
		}
	}

	void DepthPass::EndPass(RenderPassParams& params)
	{
		if (params.renderingData->renderScene->SetPasses.size() == 0)
			return;

		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID depthTextureID = renderTarget->GetDepthTexture();

		// Make sure the RT is valid
		if (!depthTextureID.IsValid())
		{
			Log::Error("[ShadowPass] Invalid render target for Shadow Pass");
			return;
		}

		// Transition the shadowmap for use in the fragment shader
		Ref<VulkanTexture> depthTexture = ResourceManager::GetResource<VulkanTexture>(depthTextureID);
		commandBuffer->TransitionLayouts(depthTexture->GetImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

		params.DepthTextures[m_Camera] = depthTextureID;
	}

	RenderObjectsPass::RenderObjectsPass()
	{
		m_ClearValue = glm::vec4(0, 0, 0, 1);
		m_SubPasses.push_back(std::make_shared<SkyboxSubPass>());
		m_SubPasses.push_back(std::make_shared<RenderObjectSubPass>(RenderQueue::Opaque));

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void RenderObjectsPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		ResourceID colorAttachmentImage;
		uint32_t width = 0;
		uint32_t height = 0;

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();
		ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();

		// Extract the render target and width/height
		if (colorTextureID.IsValid())
		{
			Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
			colorAttachmentImage = colorTexture->GetImage();
			width = colorTexture->GetWidth();
			height = colorTexture->GetHeight();

			commandBuffer->TransitionLayouts(colorAttachmentImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			// Check if msaa is enabled
			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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

			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());

				color_attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
				color_attachment_info.resolveImageView = resolveImage->GetImageView();
				color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_attachment_info.clearValue = clearValue;

			attachments.push_back(color_attachment_info);
		}


		ResourceID depthTextureID = renderTarget->GetDepthTexture();
		ResourceID depthResolveTextureID = renderTarget->GetDepthResolveTexture();

		if (depthTextureID.IsValid())
		{
			VkClearValue clearValue;
			if (Renderer::ReverseDepthEnabled())
				clearValue.depthStencil = { 0.0f, 0 };
			else
				clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			Ref<VulkanTexture> depthTexture = ResourceManager::GetResource<VulkanTexture>(depthTextureID);
			Ref<VulkanImage> depthImage = ResourceManager::GetResource<VulkanImage>(depthTexture->GetImage());
			ResourceID depthAttachmentID = depthTexture->GetImage();

			// Load the image
			auto depthAttachment = ResourceManager::GetResource<VulkanImage>(depthAttachmentID);

			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthAttachment->GetImageView();
			depthAttachmentInfo.imageLayout = depthAttachment->GetLayout();
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;

			if (depthResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(depthResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());
				depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_MIN_BIT;
				depthAttachmentInfo.resolveImageView = resolveImage->GetImageView();
				depthAttachmentInfo.resolveImageLayout = resolveImage->GetLayout();
			}

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
			viewport.y = height;
			viewport.width = (float)width;
			viewport.height = -1.0f * (float)height;
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

	void RenderObjectsPass::Execute(RenderPassParams& params)
	{
		std::shared_ptr<RenderScene> renderScene = params.renderingData->renderScene;

		RenderSubPassData subPassData;
		subPassData.CameraTag = m_Camera;

		if (!renderScene->GetCamera(m_Camera))
			return;

		// Check for a valid camera data index
		if (subPassData.CameraTag < RenderScene::MAX_CAMERAS)
		{
			for (auto& renderSubPass : m_SubPasses)
			{
				renderSubPass->Execute(params, subPassData);
			}
		}
	}

	void RenderObjectsPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		// Make sure the RT is valid
		if (!colorTextureID.IsValid())
		{
			Log::Error("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();
		if (colorResolveTextureID.IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
			commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		params.ColorTextures[m_Camera] = colorTextureID;
	}

	void RenderObjectsPass::AddDebugSubPass()
	{
		std::shared_ptr<DebugSubPass> debugSubPass = std::make_shared<DebugSubPass>();
		debugSubPass->Setup();
		m_SubPasses.push_back(debugSubPass);
	}

	void RenderObjectsPass::Add2DSubPass()
	{
		auto subPass = std::make_shared<Opaque2DSubPass>();
		subPass->Setup();
		m_SubPasses.push_back(subPass);
	}

	TransparentObjectsPass::TransparentObjectsPass()
	{
		m_SubPasses.push_back(std::make_shared<RenderObjectSubPass>(RenderQueue::Transparent));
		m_SubPasses.push_back(std::make_shared<ParticleSubPass>());

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void TransparentObjectsPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		ResourceID colorAttachmentImage;
		uint32_t width = 0;
		uint32_t height = 0;

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();

		// Extract the render target and width/height
		if (colorTextureID.IsValid())
		{
			Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
			colorAttachmentImage = colorTexture->GetImage();
			width = colorTexture->GetWidth();
			height = colorTexture->GetHeight();

			if (m_CameraColorTexture.IsValid())
			{
				Ref<VulkanTexture> cameraColor = ResourceManager::GetResource<VulkanTexture>(m_CameraColorTexture);

				if (cameraColor->GetWidth() != width || cameraColor->GetHeight() != height)
				{
					ResourceManager::Destroy(m_CameraColorTexture);
					m_CameraColorTexture = ResourceID::Invalid();
				}
			}

			if (!m_CameraColorTexture.IsValid())
			{
				VulkanImageDescription desc;
				desc.ImageType = ImageType::RenderTexture;
				desc.Width = width;
				desc.Height = height;
				desc.Format = TextureFormat::R8G8B8A8_UNORM;
				desc.Samples = 1;
				m_CameraColorTexture = ResourceManager::Allocate<VulkanTexture>(desc, nullptr);
			}

			Ref<VulkanTexture> cameraColor = ResourceManager::GetResource<VulkanTexture>(m_CameraColorTexture);

			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				resolveTexture->CopyToTexture(cameraColor->GetImage());
			}
			else
			{
				colorTexture->CopyToTexture(cameraColor->GetImage());
			}

			params.ColorTextures[m_Camera] = m_CameraColorTexture;

			commandBuffer->TransitionLayouts(colorAttachmentImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			// Check if msaa is enabled
			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
		}
		else
		{
			Log::Error("(OpaquePass) Invalid render target for opaque pass.");
			return;
		}

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

			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());

				color_attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
				color_attachment_info.resolveImageView = resolveImage->GetImageView();
				color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			attachments.push_back(color_attachment_info);
		}

		ResourceID depthTextureID = renderTarget->GetDepthTexture();
		ResourceID depthResolveTextureID = renderTarget->GetDepthResolveTexture();

		if (depthTextureID.IsValid())
		{
			VkClearValue clearValue;
			if (Renderer::ReverseDepthEnabled())
				clearValue.depthStencil = { 0.0f, 0 };
			else
				clearValue.depthStencil = { 1.0f, 0 };

			// Get the render texture's image
			Ref<VulkanTexture> depthTexture = ResourceManager::GetResource<VulkanTexture>(depthTextureID);
			Ref<VulkanImage> depthImage = ResourceManager::GetResource<VulkanImage>(depthTexture->GetImage());
			ResourceID depthAttachmentID = depthTexture->GetImage();

			// Load the image
			auto depthAttachment = ResourceManager::GetResource<VulkanImage>(depthAttachmentID);

			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthAttachment->GetImageView();
			depthAttachmentInfo.imageLayout = depthAttachment->GetLayout();
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;

			if (depthResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(depthResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());
				depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_MIN_BIT;
				depthAttachmentInfo.resolveImageView = resolveImage->GetImageView();
				depthAttachmentInfo.resolveImageLayout = resolveImage->GetLayout();
			}

			depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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
			viewport.y = height;
			viewport.width = (float)width;
			viewport.height = -1.0f * (float)height;
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

	void TransparentObjectsPass::Execute(RenderPassParams& params)
	{
		std::shared_ptr<RenderScene> renderScene = params.renderingData->renderScene;

		RenderSubPassData subPassData;
		subPassData.CameraTag = m_Camera;

		if (!renderScene->GetCamera(m_Camera))
			return;

		// Check for a valid camera data index
		if (subPassData.CameraTag < RenderScene::MAX_CAMERAS)
		{
			for (auto& renderSubPass : m_SubPasses)
			{
				renderSubPass->Execute(params, subPassData);
			}
		}
	}

	void TransparentObjectsPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		// Make sure the RT is valid
		if (!colorTextureID.IsValid())
		{
			Log::Error("(OpaquePass) Invalid render target for OpaquePass");
			return;
		}

		// Transition the backbuffer layout for presenting
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();
		if (colorResolveTextureID.IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
			commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

	}

	void ImguiPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		uint32_t width = params.renderingData->width;
		uint32_t height = params.renderingData->height;

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(params.FrameTexture);
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorTexture());
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

		if (renderTarget->GetColorResolveTexture().IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorResolveTexture());
			Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());
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
			viewport.y = (float)height;
			viewport.width = (float)width;
			viewport.height = -1.0f * (float)height;
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
		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(params.FrameTexture);
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(renderTarget->GetColorTexture());
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), m_EndLayout);

		ResourceID resolveTextureID = renderTarget->GetColorResolveTexture();
		if (resolveTextureID.IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(resolveTextureID);
			commandBuffer->TransitionLayouts(resolveTexture->GetImage(), m_EndLayout);
		}
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}
}