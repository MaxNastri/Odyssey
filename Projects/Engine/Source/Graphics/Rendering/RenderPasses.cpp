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
	void RenderPass::BeginRendering(RenderPassParams& params)
	{
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);

		uint32_t width = 0;
		uint32_t height = 0;

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		std::vector<VkRenderingAttachmentInfoKHR> attachments;

		int32_t colorAttachmentIndex = -1;

		// Extract the render target and width/height
		if (colorTextureID.IsValid())
		{
			// Get the color texture and resolve (if it has one)
			Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
			Ref<VulkanImage> colorImage = ResourceManager::GetResource<VulkanImage>(colorTexture->GetImage());
			ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();

			// Set the w/h
			width = colorTexture->GetWidth();
			height = colorTexture->GetHeight();

			// Transition the color attachment image
			commandBuffer->TransitionLayouts(colorTexture->GetImage(), m_ColorAttachment.BeginLayout);

			// Transition the resolve texture
			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				commandBuffer->TransitionLayouts(resolveTexture->GetImage(), m_ColorAttachment.BeginLayout);
			}

			// Construct the attachment info
			VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
			colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			colorAttachmentInfo.pNext = VK_NULL_HANDLE;
			colorAttachmentInfo.imageView = colorImage->GetImageView();
			colorAttachmentInfo.imageLayout = m_ColorAttachment.BeginLayout;
			colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;

			// Set the resolve image, if valid
			if (colorResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());

				colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
				colorAttachmentInfo.resolveImageView = resolveImage->GetImageView();
				colorAttachmentInfo.resolveImageLayout = m_ColorAttachment.BeginLayout;
			}

			colorAttachmentInfo.loadOp = m_ColorAttachment.LoadOp;
			colorAttachmentInfo.storeOp = m_ColorAttachment.StoreOp;
			colorAttachmentInfo.clearValue = m_ColorAttachment.ClearValue;

			attachments.push_back(colorAttachmentInfo);
			colorAttachmentIndex = attachments.size() - 1;
		}

		ResourceID depthTextureID = renderTarget->GetDepthTexture();
		int32_t depthAttachmentIndex = -1;
		bool bindStencil = false;

		if (depthTextureID.IsValid())
		{
			// Get the depth texture, resolve texture and images
			Ref<VulkanTexture> depthTexture = ResourceManager::GetResource<VulkanTexture>(depthTextureID);
			Ref<VulkanImage> depthImage = ResourceManager::GetResource<VulkanImage>(depthTexture->GetImage());
			ResourceID depthResolveTextureID = renderTarget->GetDepthResolveTexture();

			// Set the w/h
			width = depthTexture->GetWidth();
			height = depthTexture->GetHeight();
			bindStencil = depthTexture->GetFormat() == TextureFormat::D24_UNORM_S8_UINT;

			// Transition the color attachment image
			commandBuffer->TransitionLayouts(depthTexture->GetImage(), m_DepthAttachment.BeginLayout);

			// Transition the resolve texture
			if (depthResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(depthResolveTextureID);
				commandBuffer->TransitionLayouts(resolveTexture->GetImage(), m_DepthAttachment.BeginLayout);
			}

			VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
			depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
			depthAttachmentInfo.pNext = VK_NULL_HANDLE;
			depthAttachmentInfo.imageView = depthImage->GetImageView();
			depthAttachmentInfo.imageLayout = m_DepthAttachment.BeginLayout;
			depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;

			if (depthResolveTextureID.IsValid())
			{
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(depthResolveTextureID);
				Ref<VulkanImage> resolveImage = ResourceManager::GetResource<VulkanImage>(resolveTexture->GetImage());
				depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_MIN_BIT;
				depthAttachmentInfo.resolveImageView = resolveImage->GetImageView();
				depthAttachmentInfo.resolveImageLayout = m_DepthAttachment.BeginLayout;
			}

			depthAttachmentInfo.loadOp = m_DepthAttachment.LoadOp;
			depthAttachmentInfo.storeOp = m_DepthAttachment.StoreOp;
			depthAttachmentInfo.clearValue = m_DepthAttachment.ClearValue;
			attachments.push_back(depthAttachmentInfo);
			depthAttachmentIndex = attachments.size() - 1;
		}

		// Rendering info
		VkRenderingInfoKHR rendering_info = {};
		rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		rendering_info.pNext = VK_NULL_HANDLE;
		rendering_info.flags = 0;
		rendering_info.renderArea = VkRect2D{ VkOffset2D{}, VkExtent2D{width, height} };
		rendering_info.layerCount = 1;
		rendering_info.viewMask = 0;
		rendering_info.colorAttachmentCount = colorAttachmentIndex >= 0 ? 1 : 0;
		rendering_info.pColorAttachments = colorAttachmentIndex >= 0 ? &attachments[colorAttachmentIndex] : VK_NULL_HANDLE;
		rendering_info.pDepthAttachment = depthAttachmentIndex >= 0 ? &attachments[depthAttachmentIndex] : VK_NULL_HANDLE;
		rendering_info.pStencilAttachment = depthAttachmentIndex >= 0 && bindStencil ? &attachments[depthAttachmentIndex] : VK_NULL_HANDLE;

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

	BRDFLutPass::BRDFLutPass()
	{
		VulkanImageDescription imageDesc;
		imageDesc.Width = Texture_Size;
		imageDesc.Height = Texture_Size;
		imageDesc.Format = TextureFormat::R16G16_SFLOAT;
		imageDesc.ImageType = ImageType::RenderTexture;
		imageDesc.Samples = 1;

		m_RenderTarget = ResourceManager::Allocate<RenderTarget>(imageDesc, RenderTargetFlags::Color);

		m_SubPasses.push_back(std::make_shared<BRDFLutSubPass>());


		for (auto& renderSubPass : m_SubPasses)
		{
			renderSubPass->Setup();
		}
	}

	void BRDFLutPass::BeginPass(RenderPassParams& params)
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
		}
		else
		{
			Log::Error("(OpaquePass) Invalid render target for opaque pass.");
			return;
		}

		// Transfer the clear value into vulkan type
		VkClearValue clearValue;
		clearValue.color.float32[0] = 0.0f;
		clearValue.color.float32[1] = 0.0f;
		clearValue.color.float32[2] = 0.0f;
		clearValue.color.float32[3] = 1.0f;

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
			color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_attachment_info.clearValue = clearValue;

			attachments.push_back(color_attachment_info);
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
		rendering_info.pDepthAttachment = VK_NULL_HANDLE;
		rendering_info.pStencilAttachment = VK_NULL_HANDLE;

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

	void BRDFLutPass::Execute(RenderPassParams& params)
	{
		RenderSubPassData subPassData;

		for (auto& renderSubPass : m_SubPasses)
		{
			renderSubPass->Execute(params, subPassData);
		}
	}

	void BRDFLutPass::EndPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		// End dynamic rendering
		commandBuffer->EndRendering();

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		// Transition the to a shader resource layout
		Ref<VulkanTexture> colorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		params.BRDFLutTexture = colorTextureID;
	}

	DepthPass::DepthPass(uint8_t cameraTag)
	{
		m_Camera = cameraTag;

		// Set up the depth attachment info
		float depthClear = Renderer::ReverseDepthEnabled() ? 0.0f : 1.0f;
		m_DepthAttachment.ClearValue.depthStencil = { depthClear, 0 };
		m_DepthAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_DepthAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		m_DepthAttachment.BeginLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_DepthAttachment.EndLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		// Create the render target at default size
		CreateRenderTarget(Texture_Size, Texture_Size);

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

		if (Camera* camera = params.renderingData->renderScene->GetCamera(m_Camera))
		{
			if (camera->GetViewportWidth() != m_Width || camera->GetViewportHeight() != m_Height)
				CreateRenderTarget(camera->GetViewportWidth(), camera->GetViewportHeight());
		}

		BeginRendering(params);
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

	void DepthPass::CreateRenderTarget(uint32_t width, uint32_t height)
	{
		VulkanImageDescription imageDesc;
		imageDesc.Width = m_Width = Texture_Size;
		imageDesc.Height = m_Height = Texture_Size;
		imageDesc.Format = TextureFormat::D32_SFLOAT;
		imageDesc.ImageType = ImageType::Shadowmap;
		imageDesc.Samples = 1;

		if (m_RenderTarget.IsValid())
			ResourceManager::Destroy(m_RenderTarget);

		m_RenderTarget = ResourceManager::Allocate<RenderTarget>(imageDesc, RenderTargetFlags::Depth);
	}

	RenderObjectsPass::RenderObjectsPass()
	{
		m_SubPasses.push_back(std::make_shared<SkyboxSubPass>());
		m_SubPasses.push_back(std::make_shared<RenderObjectSubPass>(RenderQueue::Opaque));

		m_ColorAttachment.BeginLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorAttachment.ClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_ColorAttachment.EndLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_ColorAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_ColorAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;

		float depthClear = Renderer::ReverseDepthEnabled() ? 0.0f : 1.0f;
		m_DepthAttachment.ClearValue.depthStencil = { depthClear, 0 };
		m_DepthAttachment.BeginLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_DepthAttachment.EndLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		m_DepthAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_DepthAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void RenderObjectsPass::BeginPass(RenderPassParams& params)
	{
		BeginRendering(params);
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

		m_ColorAttachment.BeginLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorAttachment.ClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_ColorAttachment.EndLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_ColorAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		m_ColorAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;

		float depthClear = Renderer::ReverseDepthEnabled() ? 0.0f : 1.0f;
		m_DepthAttachment.ClearValue.depthStencil = { depthClear, 0 };
		m_DepthAttachment.BeginLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_DepthAttachment.EndLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		m_DepthAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		m_DepthAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;

		for (auto& subPass : m_SubPasses)
		{
			subPass->Setup();
		}
	}

	void TransparentObjectsPass::BeginPass(RenderPassParams& params)
	{
		ResourceID commandBufferID = params.GraphicsCommandBuffer;
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		Ref<RenderTarget> renderTarget = ResourceManager::GetResource<RenderTarget>(m_RenderTarget);
		ResourceID colorTextureID = renderTarget->GetColorTexture();

		// Extract the render target and width/height
		if (colorTextureID.IsValid())
		{
			Ref<VulkanTexture> rtColorTexture = ResourceManager::GetResource<VulkanTexture>(colorTextureID);
			
			if (m_CameraColorTexture.IsValid())
			{
				Ref<VulkanTexture> cameraColorTexture = ResourceManager::GetResource<VulkanTexture>(m_CameraColorTexture);
				if (cameraColorTexture->GetWidth() != rtColorTexture->GetWidth() || cameraColorTexture->GetHeight() != rtColorTexture->GetHeight())
				{
					ResourceManager::Destroy(m_CameraColorTexture);
					m_CameraColorTexture = ResourceID::Invalid();
				}
			}

			if (!m_CameraColorTexture.IsValid())
			{
				VulkanImageDescription desc;
				desc.ImageType = ImageType::RenderTexture;
				desc.Width = rtColorTexture->GetWidth();
				desc.Height = rtColorTexture->GetHeight();
				desc.Format = TextureFormat::R8G8B8A8_UNORM;
				desc.Samples = 1;
				m_CameraColorTexture = ResourceManager::Allocate<VulkanTexture>(desc, nullptr);
			}

			Ref<VulkanTexture> cameraColorTexture = ResourceManager::GetResource<VulkanTexture>(m_CameraColorTexture);
			ResourceID colorResolveTextureID = renderTarget->GetColorResolveTexture();

			if (colorResolveTextureID.IsValid())
			{
				// Copy the resolve texture into the camera's color texture
				Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(colorResolveTextureID);
				resolveTexture->CopyToTexture(cameraColorTexture->GetImage());
			}
			else
			{
				// Copy the non-msaa texture into the camera's color texture
				rtColorTexture->CopyToTexture(cameraColorTexture->GetImage());
			}

			params.ColorTextures[m_Camera] = m_CameraColorTexture;
			commandBuffer->TransitionLayouts(cameraColorTexture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		BeginRendering(params);
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

	ImguiPass::ImguiPass()
	{
		m_ColorAttachment.BeginLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorAttachment.ClearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_ColorAttachment.EndLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		m_ColorAttachment.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_ColorAttachment.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;

	}

	void ImguiPass::BeginPass(RenderPassParams& params)
	{
		m_RenderTarget = params.FrameTexture;
		BeginRendering(params);
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
		commandBuffer->TransitionLayouts(colorTexture->GetImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		ResourceID resolveTextureID = renderTarget->GetColorResolveTexture();
		if (resolveTextureID.IsValid())
		{
			Ref<VulkanTexture> resolveTexture = ResourceManager::GetResource<VulkanTexture>(resolveTextureID);
			commandBuffer->TransitionLayouts(resolveTexture->GetImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		}
	}

	void ImguiPass::SetImguiState(std::shared_ptr<VulkanImgui> imgui)
	{
		m_Imgui = imgui;
	}
}