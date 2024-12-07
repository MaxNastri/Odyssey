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
#include "Texture2D.h"
#include "Light.h"

namespace Odyssey
{
	void ShadowSubPass::Setup()
	{
		m_PushDescriptors = new VulkanPushDescriptors();
		m_Shader = AssetManager::LoadAsset<Shader>(Shader_GUID);
		m_SkinnedShader = AssetManager::LoadAsset<Shader>(Skinned_Shader_GUID);

		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();

		Ref<VulkanDescriptorLayout> descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->AddBinding("Model Data", DescriptorType::Uniform, ShaderStage::Vertex, 1);
		descriptorLayout->AddBinding("Skinning Data", DescriptorType::Uniform, ShaderStage::Vertex, 2);
		descriptorLayout->Apply();

		// Non-skinned pipeline
		{
			VulkanPipelineInfo info;
			info.Shaders = m_Shader->GetResourceMap();
			info.CullMode = CullMode::Front;
			info.DescriptorLayout = m_DescriptorLayout;
			info.MSAACountOverride = 1;
			GetAttributeDescriptions(info.AttributeDescriptions, false);

			m_Pipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		}

		// Skinned pipeline
		{
			VulkanPipelineInfo info;
			info.Shaders = m_SkinnedShader->GetResourceMap();
			info.CullMode = CullMode::Front;
			info.DescriptorLayout = m_DescriptorLayout;
			info.MSAACountOverride = 1;
			GetAttributeDescriptions(info.AttributeDescriptions, true);

			m_SkinnedPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		}
	}

	void ShadowSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		auto renderScene = params.renderingData->renderScene;

		for (SetPass& setPass : renderScene->setPasses)
		{
			for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
			{
				Drawcall& drawcall = setPass.Drawcalls[i];

				// Add the camera and per object data to the push descriptors
				uint32_t uboIndex = drawcall.UniformBufferIndex;
				m_PushDescriptors->Clear();
				m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[uboIndex], 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);

				if (drawcall.Skinned)
					m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);

				commandBuffer->SetDepthBias(100.25f, 0.0f, 1.75f);

				// Push the descriptors into the command buffer
				if (drawcall.Skinned)
				{
					commandBuffer->BindGraphicsPipeline(m_SkinnedPipeline);
					commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_SkinnedPipeline);
				}
				else
				{
					commandBuffer->BindGraphicsPipeline(m_Pipeline);
					commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_Pipeline);
				}

				// Set the per-object descriptor buffer offset
				commandBuffer->BindVertexBuffer(drawcall.VertexBufferID);
				commandBuffer->BindIndexBuffer(drawcall.IndexBufferID);
				commandBuffer->DrawIndexed(drawcall.IndexCount, 1, 0, 0, 0);
			}
		}
	}

	void ShadowSubPass::GetAttributeDescriptions(BinaryBuffer& attributeDescriptions, bool skinned)
	{
		std::vector<VkVertexInputAttributeDescription> descriptions;

		// Position
		auto& positionDesc = descriptions.emplace_back();
		positionDesc.binding = 0;
		positionDesc.location = 0;
		positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionDesc.offset = offsetof(Vertex, Position);

		// Normal
		auto& normalDesc = descriptions.emplace_back();
		normalDesc.binding = 0;
		normalDesc.location = 1;
		normalDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalDesc.offset = offsetof(Vertex, Normal);

		if (skinned)
		{
			// Bone Indices
			auto& indicesDesc = descriptions.emplace_back();
			indicesDesc.binding = 0;
			indicesDesc.location = 2;
			indicesDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			indicesDesc.offset = offsetof(Vertex, BoneIndices);

			// Bone Weights
			auto& weightsDesc = descriptions.emplace_back();
			weightsDesc.binding = 0;
			weightsDesc.location = 3;
			weightsDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			weightsDesc.offset = offsetof(Vertex, BoneWeights);
		}

		attributeDescriptions.WriteData(descriptions);
	}

	void OpaqueSubPass::Setup()
	{
		m_PushDescriptors = new VulkanPushDescriptors();

		m_BlackTexture = AssetManager::LoadAsset<Texture2D>(s_BlackTextureGUID);
		m_BlackTextureID = m_BlackTexture->GetTexture();

		m_WhiteTexture = AssetManager::LoadAsset<Texture2D>(s_WhiteTextureGUID);
		m_WhiteTextureID = m_WhiteTexture->GetTexture();
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
				m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraIndex], 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);
				m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);
				m_PushDescriptors->AddBuffer(renderScene->LightingBuffer, 3);

				// Color map binds to the fragment shader register 4
				if (setPass.ColorTexture.IsValid())
					m_PushDescriptors->AddTexture(setPass.ColorTexture, 4);
				else
					m_PushDescriptors->AddTexture(m_BlackTextureID, 4);

				// Normal map binds to the fragment shader register 5
				if (setPass.NormalTexture.IsValid())
					m_PushDescriptors->AddTexture(setPass.NormalTexture, 5);
				else
					m_PushDescriptors->AddTexture(m_BlackTextureID, 5);

				// Shadowmap binds to the fragment shader register 6
				if (params.Shadowmap.IsValid())
					m_PushDescriptors->AddRenderTexture(params.Shadowmap, 6);
				else
					m_PushDescriptors->AddTexture(m_WhiteTextureID, 6);

				// Push the descriptors into the command buffer
				commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), setPass.GraphicsPipeline);

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
		GetAttributeDescriptions(info.AttributeDescriptions);

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = new VulkanPushDescriptors();
	}

	void DebugSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);

		// Bind our graphics pipeline
		commandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);

		// Push the camera descriptors
		auto renderScene = params.renderingData->renderScene;
		m_PushDescriptors->Clear();
		m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraIndex], 0);

		// Push the descriptors into the command buffer
		commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_GraphicsPipeline);

		// Set the per-object descriptor buffer offset
		commandBuffer->BindVertexBuffer(DebugRenderer::GetVertexBuffer());
		commandBuffer->Draw((uint32_t)DebugRenderer::GetVertexCount(), 1, 0, 0);
	}

	void DebugSubPass::GetAttributeDescriptions(BinaryBuffer& attributeDescriptions)
	{
		std::vector<VkVertexInputAttributeDescription> descriptions;

		// Position
		auto& positionDesc = descriptions.emplace_back();
		positionDesc.binding = 0;
		positionDesc.location = 0;
		positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionDesc.offset = offsetof(Vertex, Position);

		// Normal
		auto& normalDesc = descriptions.emplace_back();
		normalDesc.binding = 0;
		normalDesc.location = 1;
		normalDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalDesc.offset = offsetof(Vertex, Normal);

		// Tangent
		auto& tangentDesc = descriptions.emplace_back();
		tangentDesc.binding = 0;
		tangentDesc.location = 2;
		tangentDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		tangentDesc.offset = offsetof(Vertex, Tangent);

		// Color
		auto& colorDesc = descriptions.emplace_back();
		colorDesc.binding = 0;
		colorDesc.location = 3;
		colorDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		colorDesc.offset = offsetof(Vertex, Color);

		// TexCoord0
		auto& texCoord0Desc = descriptions.emplace_back();
		texCoord0Desc.binding = 0;
		texCoord0Desc.location = 4;
		texCoord0Desc.format = VK_FORMAT_R32G32_SFLOAT;
		texCoord0Desc.offset = offsetof(Vertex, TexCoord0);

		attributeDescriptions.WriteData(descriptions);
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
		info.CullMode = CullMode::None;
		GetAttributeDescriptions(info.AttributeDescriptions);

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = new VulkanPushDescriptors();

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
		m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraIndex], 0);
		m_PushDescriptors->AddBuffer(uboID, 1);

		m_PushDescriptors->AddTexture(renderScene->SkyboxCubemap, 3);

		// Push the descriptors into the command buffer
		commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_GraphicsPipeline);

		commandBuffer->BindVertexBuffer(m_CubeMesh->GetVertexBuffer());
		commandBuffer->BindIndexBuffer(m_CubeMesh->GetIndexBuffer());
		commandBuffer->DrawIndexed(m_CubeMesh->GetIndexCount(), 1, 0, 0, 0);
	}

	void SkyboxSubPass::GetAttributeDescriptions(BinaryBuffer& attributeDescriptions)
	{
		std::vector<VkVertexInputAttributeDescription> descriptions;

		// Position
		auto& positionDesc = descriptions.emplace_back();
		positionDesc.binding = 0;
		positionDesc.location = 0;
		positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionDesc.offset = offsetof(Vertex, Position);

		attributeDescriptions.WriteData(descriptions);
	}

	void ParticleSubPass::Setup()
	{
		// Create the descriptor layout
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();
		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->AddBinding("Particle Buffer", DescriptorType::Storage, ShaderStage::Vertex, 2);
		descriptorLayout->AddBinding("Particle Texture", DescriptorType::Sampler, ShaderStage::Fragment, 3);
		descriptorLayout->AddBinding("Alive Pre-Sim Buffer", DescriptorType::Storage, ShaderStage::Vertex, 4);
		descriptorLayout->Apply();

		m_ModelUBO = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(glm::mat4));
		m_Shader = AssetManager::LoadAsset<Shader>(s_ParticleShaderGUID);
		m_ParticleTexture = AssetManager::LoadAsset<Texture2D>(s_ParticleTextureGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_DescriptorLayout;
		info.BindVertexAttributeDescriptions = false;
		info.AlphaBlend = true;
		info.WriteDepth = false;

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = new VulkanPushDescriptors();
	}

	void ParticleSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto renderScene = params.renderingData->renderScene;
		auto graphicsCommandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);

		const std::vector<size_t>& drawList = ParticleBatcher::GetDrawList();

		for (size_t index : drawList)
		{
			uint32_t aliveCount = ParticleBatcher::GetAliveCount(index);
			ResourceID particleBuffer = ParticleBatcher::GetParticleBuffer(index);
			ResourceID aliveBuffer = ParticleBatcher::GetAliveBuffer(index);

			m_PushDescriptors->Clear();
			m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraIndex], 0);
			m_PushDescriptors->AddBuffer(particleBuffer, 2);
			m_PushDescriptors->AddTexture(m_ParticleTexture->GetTexture(), 3);
			m_PushDescriptors->AddBuffer(aliveBuffer, 4);

			graphicsCommandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);
			graphicsCommandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_GraphicsPipeline);
			graphicsCommandBuffer->Draw(aliveCount * 6, 1, 0, 0);
		}
	}
}