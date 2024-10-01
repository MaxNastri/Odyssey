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
#include "Mesh.h"
#include "AssetManager.h"
#include "VulkanComputePipeline.h"
#include "ParticleBatcher.h"
#include "VulkanBuffer.h"

namespace Odyssey
{
	void OpaqueSubPass::Setup()
	{
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void OpaqueSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		auto renderScene = params.renderingData->renderScene;

		for (auto& setPass : params.renderingData->renderScene->setPasses)
		{
			commandBuffer->BindGraphicsPipeline(setPass.GraphicsPipeline);

			for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
			{
				Drawcall& drawcall = setPass.Drawcalls[i];

				// Add the camera and per object data to the push descriptors
				uint32_t uboIndex = drawcall.UniformBufferIndex;
				m_PushDescriptors->Clear();
				m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);
				m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);
				m_PushDescriptors->AddBuffer(renderScene->LightingBuffer, 3);

				// Add textures, if they are set
				if (setPass.Texture.IsValid())
					m_PushDescriptors->AddTexture(setPass.Texture, 4);

				// Push the descriptors into the command buffer
				commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.get(), setPass.GraphicsPipeline);

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

		m_Shader = AssetManager::LoadAsset<Shader>(s_DebugShaderGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_DescriptorLayout;
		info.Topology = Topology::LineList;

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void DebugSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);

		// Bind our graphics pipeline
		commandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);

		// Push the camera descriptors
		auto renderScene = params.renderingData->renderScene;
		m_PushDescriptors->Clear();
		m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);

		// Push the descriptors into the command buffer
		commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.get(), m_GraphicsPipeline);

		// Set the per-object descriptor buffer offset
		commandBuffer->BindVertexBuffer(DebugRenderer::GetVertexBuffer());
		commandBuffer->Draw((uint32_t)DebugRenderer::GetVertexCount(), 1, 0, 0);
	}

	void SkyboxSubPass::Setup()
	{
		// Create the descriptor layout
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->AddBinding("Model Data", DescriptorType::Uniform, ShaderStage::Vertex, 1);
		descriptorLayout->AddBinding("Skybox", DescriptorType::Sampler, ShaderStage::Fragment, 3);
		descriptorLayout->Apply();

		m_Shader = AssetManager::LoadAsset<Shader>(s_SkyboxShaderGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_DescriptorLayout;
		info.WriteDepth = false;

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();

		m_CubeMesh = AssetManager::LoadAsset<Mesh>(s_CubeMeshGUID);

		// Allocate the UBO
		uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(glm::mat4));

		// Write the camera data into the ubo memory
		auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
		glm::mat4 identity = glm::mat4(1.0f);
		uniformBuffer->CopyData(sizeof(glm::mat4), &identity);
	}

	void SkyboxSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto renderScene = params.renderingData->renderScene;

		if (!renderScene->SkyboxCubemap.IsValid())
			return;

		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);

		glm::mat4 world = subPassData.Camera->GetView();
		glm::mat4 posOnly = glm::translate(glm::mat4(1.0f), glm::vec3(world[3][0], world[3][1], world[3][2]));
		auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
		uniformBuffer->CopyData(sizeof(glm::mat4), &posOnly);

		// Bind our graphics pipeline
		commandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);

		m_PushDescriptors->Clear();
		m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);
		m_PushDescriptors->AddBuffer(uboID, 1);

		m_PushDescriptors->AddTexture(renderScene->SkyboxCubemap, 3);

		// Push the descriptors into the command buffer
		commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.get(), m_GraphicsPipeline);

		commandBuffer->BindVertexBuffer(m_CubeMesh->GetVertexBuffer());
		commandBuffer->BindIndexBuffer(m_CubeMesh->GetIndexBuffer());
		commandBuffer->DrawIndexed(m_CubeMesh->GetIndexCount(), 1, 0, 0, 0);
	}

	void ParticleSubPass::Setup()
	{
		// Create the descriptor layout
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->AddBinding("Model Data", DescriptorType::Uniform, ShaderStage::Vertex, 1);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Compute, 2);
		descriptorLayout->AddBinding("Counter Buffer", DescriptorType::Storage, ShaderStage::Compute, 3);
		descriptorLayout->AddBinding("Alive Pre-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 4);
		descriptorLayout->AddBinding("Alive Post-Sim Buffer", DescriptorType::Storage, ShaderStage::Compute, 5);
		descriptorLayout->AddBinding("Dead Buffer", DescriptorType::Storage, ShaderStage::Compute, 6);
		descriptorLayout->Apply();

		m_ModelUBO = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(glm::mat4));

		m_Shader = AssetManager::LoadAsset<Shader>(s_ParticleShaderGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_DescriptorLayout;
		info.BindVertexAttributeDescriptions = false;

		m_ComputePipeline = ResourceManager::Allocate<VulkanComputePipeline>(info);
		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = std::make_shared<VulkanPushDescriptors>();
	}

	void ParticleSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		uint32_t aliveCount = ParticleBatcher::AliveCount();
		if (aliveCount > 0)
		{
			auto renderScene = params.renderingData->renderScene;
			auto computeCommandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.ComputeCommandBuffer);
			auto graphicsCommandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
			computeCommandBuffer->BeginCommands();

			glm::mat4 world = glm::mat4(1.0f);
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(m_ModelUBO);
			uniformBuffer->CopyData(sizeof(glm::mat4), &world);

			// Bind our graphics pipeline
			computeCommandBuffer->BindComputePipeline(m_ComputePipeline);

			m_PushDescriptors->Clear();
			m_PushDescriptors->AddBuffer(renderScene->cameraDataBuffers[subPassData.CameraIndex], 0);
			m_PushDescriptors->AddBuffer(m_ModelUBO, 1);
			m_PushDescriptors->AddBuffer(ParticleBatcher::GetParticleBuffer(), 2);
			m_PushDescriptors->AddBuffer(ParticleBatcher::GetCounterBuffer(), 3);
			m_PushDescriptors->AddBuffer(ParticleBatcher::GetAlivePreSimBuffer(), 4);
			m_PushDescriptors->AddBuffer(ParticleBatcher::GetAlivePostSimBuffer(), 5);
			m_PushDescriptors->AddBuffer(ParticleBatcher::GetDeadBuffer(), 6);

			computeCommandBuffer->PushDescriptorsCompute(m_PushDescriptors.get(), m_ComputePipeline);
			computeCommandBuffer->Dispatch(1, 1, 1);
			computeCommandBuffer->EndCommands();
			computeCommandBuffer->SubmitCompute();

			graphicsCommandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);
			graphicsCommandBuffer->PushDescriptorsGraphics(m_PushDescriptors.get(), m_GraphicsPipeline);
			graphicsCommandBuffer->Draw(aliveCount * 6, 1, 0, 0);
		}
	}
}