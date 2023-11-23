#include "RenderGraphNodes.h"
#include "VulkanContext.h"
#include "ResourceManager.h"
#include "VulkanTypes.h"
#include "VulkanCommandBuffer.h"
#include "VulkanTexture.h"
#include "VulkanImgui.h"

namespace Odyssey
{
	BeginPassNode::BeginPassNode(const std::string& name)
	{
		m_Name = name;
	}

	BeginPassNode::BeginPassNode(const std::string& name, ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader, ResourceHandle<VulkanTexture> renderTarget)
	{
		m_Name = name;
		m_VertexShader = vertexShader;
		m_FragmentShader = fragmentShader;
		m_RenderTarget = renderTarget;
	}

	BeginPassNode::BeginPassNode(const std::string& name, ResourceHandle<VulkanShader> vertexShader, ResourceHandle<VulkanShader> fragmentShader)
	{
		m_Name = name;
		m_VertexShader = vertexShader;
		m_FragmentShader = fragmentShader;
	}

	void BeginPassNode::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBuffer)
	{
		// Create the graphics pipeline
		if (!m_GraphicsPipeline.IsValid())
		{
			VulkanPipelineInfo info;
			info.fragmentShader = m_FragmentShader;
			info.vertexShader = m_VertexShader;
			m_GraphicsPipeline = ResourceManager::AllocateGraphicsPipeline(info);

			// Init the clear value to black
			m_ClearValue = glm::vec4(0, 0, 0, 1);
		}
	}

	void BeginPassNode::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get();

		VulkanImage* renderTarget = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;

		if (m_RenderTarget.IsValid())
		{
			VulkanTexture* renderTexture = m_RenderTarget.Get();
			renderTarget = m_RenderTarget.Get()->GetImage();
			width = renderTexture->GetWidth();
			height = renderTexture->GetHeight();
		}
		else
		{
			renderTarget = renderingData->frame->GetRenderTarget();
			width = renderingData->width;
			height = renderingData->height;
		}

		VkClearValue clearValue;
		clearValue.color.float32[0] = m_ClearValue.r;
		clearValue.color.float32[1] = m_ClearValue.g;
		clearValue.color.float32[2] = m_ClearValue.b;
		clearValue.color.float32[3] = m_ClearValue.a;

		VkRenderingAttachmentInfoKHR color_attachment_info{};
		color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.pNext = VK_NULL_HANDLE;
		color_attachment_info.imageView = renderTarget->GetImageView();
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
		commandBuffer->BindPipeline(m_GraphicsPipeline);

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

	DrawNode::DrawNode(const std::string& name)
	{
		m_Name = name;
	}

	void DrawNode::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		// nop
	}

	void DrawNode::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get();

		for (auto& drawCall : renderingData->m_Drawcalls)
		{
			commandBuffer->BindVertexBuffer(drawCall.VertexBuffer);
			commandBuffer->BindIndexBuffer(drawCall.IndexBuffer);
			commandBuffer->DrawIndexed(drawCall.IndexCount, 1, 0, 0, 0);
		}
	}

	SubmitNode::SubmitNode(const std::string& name)
	{
		m_Name = name;
	}

	void SubmitNode::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		// nop
	}

	void SubmitNode::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		// TODO: Move this into the context?
		VulkanFrame* frame = renderingData->frame;
		VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get();

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
		VkResult err = vkQueueSubmit(context->GetGraphicsQueueVK(), 1, &submitInfo, frame->fence);
		check_vk_result(err);
	}

	EndPassNode::EndPassNode(const std::string& name)
	{
		m_Name = name;
	}

	EndPassNode::EndPassNode(const std::string& name, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		m_OldLayout = oldLayout;
		m_NewLayout = newLayout;
	}

	EndPassNode::EndPassNode(const std::string& name, ResourceHandle<VulkanTexture> renderTarget, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		m_RenderTarget = renderTarget;
		m_OldLayout = oldLayout;
		m_NewLayout = newLayout;
	}

	void EndPassNode::SetLayouts(VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		m_OldLayout = oldLayout;
		m_NewLayout = newLayout;
	}

	void EndPassNode::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		// nop
	}

	void EndPassNode::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get();
		commandBuffer->EndRendering();

		VulkanImage* renderTarget = nullptr;
		if (m_RenderTarget.IsValid())
		{
			renderTarget = m_RenderTarget.Get()->GetImage();
		}
		else
		{
			renderTarget = renderingData->frame->GetRenderTarget();
		}

		// Transition the backbuffer layout for presenting
		commandBuffer->TransitionLayouts(renderTarget, m_OldLayout, m_NewLayout);
	}

	ImguiDrawNode::ImguiDrawNode(const std::string& name)
	{
		m_Name = name;
	}

	ImguiDrawNode::ImguiDrawNode(const std::string& name, std::shared_ptr<VulkanImgui> imgui)
	{
		m_Name = name;
		m_Imgui = imgui;
	}

	void ImguiDrawNode::AddDescriptorSet(VkDescriptorSet descriptorSet)
	{
		m_DescriptorSet = descriptorSet;
	}

	void ImguiDrawNode::Setup(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		//nop
	}

	void ImguiDrawNode::Execute(VulkanContext* context, PerFrameRenderingData* renderingData, ResourceHandle<VulkanCommandBuffer> commandBufferHandle)
	{
		VulkanCommandBuffer* commandBuffer = commandBufferHandle.Get();
		m_Imgui->Render(commandBuffer->GetCommandBuffer(), m_DescriptorSet);
	}
}