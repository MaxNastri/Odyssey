#include "RenderSubPasses.h"
#include "DebugRenderer.h"
#include "ResourceManager.h"
#include "RenderScene.h"
#include "Shader.h"
#include "PerFrameRenderingData.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPushDescriptors.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanGraphicsPipeline.h"

namespace Odyssey
{
	void OpaqueSubPass::Setup()
	{
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void OpaqueSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.commandBuffer);
		auto renderScene = params.renderingData->renderScene;

		for (auto& setPass : params.renderingData->renderScene->setPasses)
		{
			commandBuffer->BindPipeline(setPass.GraphicsPipeline);

			for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
			{
				Drawcall& drawcall = setPass.Drawcalls[i];

				// Add the camera and per object data to the push descriptors
				uint32_t uboIndex = drawcall.UniformBufferIndex;
				m_PushDescriptors->Clear();
				m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);
				m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);

				// Add textures, if they are set
				if (setPass.Texture.IsValid())
					m_PushDescriptors->AddTexture(setPass.Texture, 3);

				// Push the descriptors into the command buffer
				commandBuffer->PushDescriptors(m_PushDescriptors.get(), setPass.GraphicsPipeline);

				// Set the per-object descriptor buffer offset
				commandBuffer->BindVertexBuffer(drawcall.VertexBufferID);
				commandBuffer->BindIndexBuffer(drawcall.IndexBufferID);
				commandBuffer->DrawIndexed(drawcall.IndexCount, 1, 0, 0, 0);
			}
		}
	}

	void DebugSubPass::Setup()
	{
		// Create the descriptor layout
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->Apply();

		VulkanPipelineInfo info;
		info.Shaders = DebugRenderer::GetShader()->GetResourceMap();
		info.DescriptorLayout = m_DescriptorLayout;
		info.Triangles = false;

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void DebugSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.commandBuffer);

		// Bind our graphics pipeline
		commandBuffer->BindPipeline(m_GraphicsPipeline);

		// Push the camera descriptors
		auto renderScene = params.renderingData->renderScene;
		m_PushDescriptors->Clear();
		m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);

		// Push the descriptors into the command buffer
		commandBuffer->PushDescriptors(m_PushDescriptors.get(), m_GraphicsPipeline);

		// Set the per-object descriptor buffer offset
		commandBuffer->BindVertexBuffer(DebugRenderer::GetVertexBuffer());
		commandBuffer->Draw((uint32_t)DebugRenderer::GetVertexCount(), 1, 0, 0);
	}
}