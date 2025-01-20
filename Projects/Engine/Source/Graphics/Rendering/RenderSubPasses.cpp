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
#include "OdysseyTime.h"
#include "Renderer.h"

namespace Odyssey
{
	void DepthSubPass::Setup()
	{
		m_PushDescriptors = new VulkanPushDescriptors();
		m_Shader = AssetManager::LoadAsset<Shader>(Shader_GUID);
		m_SkinnedShader = AssetManager::LoadAsset<Shader>(Skinned_Shader_GUID);

		// Allocate the UBO
		m_DepthUBO = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(glm::mat4));

		// Non-skinned pipeline
		{
			VulkanPipelineInfo info;
			info.Shaders = m_Shader->GetResourceMap();
			info.CullMode = CullMode::Front;
			info.DescriptorLayout = m_Shader->GetDescriptorLayout();
			info.MSAACountOverride = 1;
			info.ColorFormat = TextureFormat::None;
			info.DepthFormat = TextureFormat::D32_SFLOAT;
			info.IsShadow = true;
			GetAttributeDescriptions(info.AttributeDescriptions, false);

			m_Pipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		}

		// Skinned pipeline
		{
			VulkanPipelineInfo info;
			info.Shaders = m_SkinnedShader->GetResourceMap();
			info.CullMode = CullMode::Front;
			info.DescriptorLayout = m_SkinnedShader->GetDescriptorLayout();
			info.MSAACountOverride = 1;
			info.ColorFormat = TextureFormat::None;
			info.DepthFormat = TextureFormat::D32_SFLOAT;
			info.IsShadow = true;
			GetAttributeDescriptions(info.AttributeDescriptions, true);

			m_SkinnedPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		}
	}

	void DepthSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		auto renderScene = params.renderingData->renderScene;

		mat4 depthMatrix = renderScene->GetShadowLightMatrix();

		// If there is a valid camera tag, use that as the depth matrix
		if (subPassData.CameraTag > 0)
		{
			if (Camera* camera = renderScene->GetCamera(subPassData.CameraTag))
				depthMatrix = camera->GetProjection() * camera->GetInverseView();
		}

		// Update the ubo
		Ref<VulkanBuffer> depthUbo = ResourceManager::GetResource<VulkanBuffer>(m_DepthUBO);
		depthUbo->CopyData(sizeof(mat4), &depthMatrix);

		for (SetPass& setPass : renderScene->SetPasses[RenderQueue::Opaque])
		{
			for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
			{
				Drawcall& drawcall = setPass.Drawcalls[i];

				// Add the camera and per object data to the push descriptors
				uint32_t uboIndex = drawcall.UniformBufferIndex;
				m_PushDescriptors->Clear();
				m_PushDescriptors->AddBuffer(m_DepthUBO, 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);

				if (drawcall.Skinned)
					m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);

				commandBuffer->SetDepthBias(-1.0f, 0.0f, -1.25f);

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

	void DepthSubPass::GetAttributeDescriptions(BinaryBuffer& attributeDescriptions, bool skinned)
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

	RenderObjectSubPass::RenderObjectSubPass(RenderQueue renderQueue)
		: m_RenderQueue(renderQueue)
	{
	}

	void RenderObjectSubPass::Setup()
	{
		m_PushDescriptors = new VulkanPushDescriptors();

		m_GlobalDataUBO = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(GlobalData));
		m_BlackTexture = AssetManager::LoadAsset<Texture2D>(s_BlackTextureGUID);
		m_BlackTextureID = m_BlackTexture->GetTexture();

		m_WhiteTexture = AssetManager::LoadAsset<Texture2D>(s_WhiteTextureGUID);
		m_WhiteTextureID = m_WhiteTexture->GetTexture();
	}

	void RenderObjectSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		auto renderScene = params.renderingData->renderScene;
		renderScene->SetSceneData(subPassData.CameraTag);

		if (Camera* camera = renderScene->GetCamera(subPassData.CameraTag))
		{
			GlobalData globalData;
			if (Renderer::ReverseDepthEnabled())
			{
				globalData.ZBufferParams.x = -1.0f + (camera->GetFarClip() / camera->GetNearClip());
				globalData.ZBufferParams.y = 1.0f;
				globalData.ZBufferParams.z = globalData.ZBufferParams.x / camera->GetFarClip();
				globalData.ZBufferParams.w = 1.0f / camera->GetFarClip();
			}
			else
			{
				globalData.ZBufferParams.x = 1.0f - (camera->GetFarClip() / camera->GetNearClip());
				globalData.ZBufferParams.y = camera->GetFarClip() / camera->GetNearClip();
				globalData.ZBufferParams.z = globalData.ZBufferParams.x / camera->GetFarClip();
				globalData.ZBufferParams.w = globalData.ZBufferParams.y / camera->GetFarClip();
			}

			//x is 1.0 (or –1.0 if currently rendering with a flipped projection matrix), y is the camera’s near plane, z is the camera’s far plane and w is 1/FarPlane.
			globalData.ProjectionParams.x = -1.0f;
			globalData.ProjectionParams.y = camera->GetNearClip();
			globalData.ProjectionParams.z = camera->GetFarClip();
			globalData.ProjectionParams.w = 1.0f / camera->GetFarClip();

			//x is the width of the camera’s target texture in pixels, y is the height of the camera’s target texture in pixels,
			// z is 1.0 + 1.0 / width and w is 1.0 + 1.0 / height.
			globalData.ScreenParams.x = camera->GetViewportWidth();
			globalData.ScreenParams.y = camera->GetViewportHeight();
			globalData.ScreenParams.z = 1.0f + (1.0f / globalData.ScreenParams.x);
			globalData.ScreenParams.w = 1.0f + (1.0f / globalData.ScreenParams.y);

			globalData.Time.x = Time::Elapsed() / 20.0f;
			globalData.Time.y = Time::Elapsed();
			globalData.Time.z = Time::Elapsed() * 2.0f;
			globalData.Time.w = Time::Elapsed() * 3.0f;

			Ref<VulkanBuffer> ubo = ResourceManager::GetResource<VulkanBuffer>(m_GlobalDataUBO);
			ubo->CopyData(sizeof(GlobalData), &globalData);
		}

		for (auto& setPass : params.renderingData->renderScene->SetPasses[m_RenderQueue])
		{
			commandBuffer->BindGraphicsPipeline(setPass.GraphicsPipeline);

			for (size_t i = 0; i < setPass.Drawcalls.size(); i++)
			{
				Drawcall& drawcall = setPass.Drawcalls[i];

				// Add the camera and per object data to the push descriptors
				uint32_t uboIndex = drawcall.UniformBufferIndex;
				m_PushDescriptors->Clear();
				m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraTag], 0);
				m_PushDescriptors->AddBuffer(renderScene->perObjectUniformBuffers[uboIndex], 1);
				m_PushDescriptors->AddBuffer(renderScene->skinningBuffers[uboIndex], 2);
				m_PushDescriptors->AddBuffer(m_GlobalDataUBO, 3);
				m_PushDescriptors->AddBuffer(renderScene->LightingBuffer, 4);
				m_PushDescriptors->AddBuffer(setPass.MaterialBuffer, 5);

				// Color map binds to the fragment shader register 6
				if (setPass.ColorTexture.IsValid())
					m_PushDescriptors->AddTexture(setPass.ColorTexture, 6);
				else
					m_PushDescriptors->AddTexture(m_BlackTextureID, 6);

				// Normal map binds to the fragment shader register 7
				if (setPass.NormalTexture.IsValid())
					m_PushDescriptors->AddTexture(setPass.NormalTexture, 7);
				else
					m_PushDescriptors->AddTexture(m_BlackTextureID, 7);

				// Noise texture binds to the fragment shader register 8
				if (setPass.NoiseTexture.IsValid())
					m_PushDescriptors->AddTexture(setPass.NoiseTexture, 8);
				else
					m_PushDescriptors->AddTexture(m_BlackTextureID, 8);

				// Shadowmap binds to the fragment shader register 9
				if (params.Shadowmap().IsValid())
					m_PushDescriptors->AddTexture(params.Shadowmap(), 9);
				else
					m_PushDescriptors->AddTexture(m_WhiteTextureID, 9);

				if (params.DepthTextures.contains(subPassData.CameraTag))
				{
					ResourceID cameraDepthTexture = params.DepthTextures[subPassData.CameraTag];
					if (cameraDepthTexture.IsValid())
						m_PushDescriptors->AddTexture(cameraDepthTexture, 10);
				}
				else
				{
					m_PushDescriptors->AddTexture(m_WhiteTextureID, 10);
				}

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
		m_Shader = AssetManager::LoadAsset<Shader>(s_DebugShaderGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
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
		m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraTag], 0);

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
		m_Shader = AssetManager::LoadAsset<Shader>(s_SkyboxShaderGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
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

		float3 viewPos = renderScene->GetCamera(subPassData.CameraTag)->GetViewPosition();
		glm::mat4 posOnly = glm::translate(glm::mat4(1.0f), viewPos);

		auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
		uniformBuffer->CopyData(sizeof(glm::mat4), &posOnly);

		// Bind our graphics pipeline
		commandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);

		m_PushDescriptors->Clear();
		m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraTag], 0);
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
		m_ModelUBO = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(glm::mat4));
		m_Shader = AssetManager::LoadAsset<Shader>(s_ParticleShaderGUID);
		m_ParticleTexture = AssetManager::LoadAsset<Texture2D>(s_ParticleTextureGUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
		info.BindVertexAttributeDescriptions = false;
		info.AlphaBlend = true;
		info.WriteDepth = false;

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = new VulkanPushDescriptors();
	}

	void ParticleSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		Ref<VulkanCommandBuffer> graphicsCommandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		auto renderScene = params.renderingData->renderScene;
		renderScene->SetSceneData(subPassData.CameraTag);

		const std::vector<size_t>& drawList = ParticleBatcher::GetDrawList();

		for (size_t index : drawList)
		{
			uint32_t aliveCount = ParticleBatcher::GetAliveCount(index);
			ResourceID particleBuffer = ParticleBatcher::GetParticleBuffer(index);
			ResourceID aliveBuffer = ParticleBatcher::GetAliveBuffer(index);
			GUID materialGUID = ParticleBatcher::GetMaterial(index);

			ResourceID colorTexture = m_ParticleTexture->GetTexture();

			if (Ref<Material> material = AssetManager::LoadAsset<Material>(materialGUID))
			{
				if (Ref<Texture2D> texture = material->GetColorTexture())
					colorTexture = texture->GetTexture();
			}

			m_PushDescriptors->Clear();
			m_PushDescriptors->AddBuffer(renderScene->sceneDataBuffers[subPassData.CameraTag], 0);
			m_PushDescriptors->AddBuffer(particleBuffer, 2);
			m_PushDescriptors->AddTexture(colorTexture, 3);
			m_PushDescriptors->AddBuffer(aliveBuffer, 4);

			graphicsCommandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);
			graphicsCommandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_GraphicsPipeline);
			graphicsCommandBuffer->Draw(aliveCount * 6, 1, 0, 0);
		}
	}

	void Opaque2DSubPass::Setup()
	{
		for (size_t i = 0; i < Max_Supported_Sprites; i++)
			m_SpriteDataUBO[i] = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(SpriteData));

		m_Shader = AssetManager::LoadAsset<Shader>(Shader_GUID);
		m_Shader->AddOnModifiedListener([this]() { OnSpriteShaderModified(); });

		m_QuadMesh = AssetManager::LoadAsset<Mesh>(Quad_Mesh_GUID);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
		info.BindVertexAttributeDescriptions = true;
		info.AlphaBlend = false;
		info.WriteDepth = true;
		GetAttributeDescriptions(info.AttributeDescriptions);

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
		m_PushDescriptors = new VulkanPushDescriptors();
	}

	void Opaque2DSubPass::Execute(RenderPassParams& params, RenderSubPassData& subPassData)
	{
		auto renderScene = params.renderingData->renderScene;

		Ref<VulkanCommandBuffer> commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(params.GraphicsCommandBuffer);
		Camera* camera = renderScene->GetCamera(subPassData.CameraTag);

		assert(renderScene->SpriteDrawcalls.size() < Max_Supported_Sprites);
		assert(camera);

		mat4 orthoProjection = camera->GetScreenSpaceProjection();
		float width = camera->GetViewportWidth();
		float height = camera->GetViewportHeight();

		for (size_t i = 0; i < renderScene->SpriteDrawcalls.size(); i++)
		{
			SpriteDrawcall& spriteDrawcall = renderScene->SpriteDrawcalls[i];
			SpriteData spriteData = {};

			float2 anchor = float2(0.0f);
			if (spriteDrawcall.Anchor == SpriteRenderer::AnchorPosition::BottomRight)
				anchor.x = width;
			else if (spriteDrawcall.Anchor == SpriteRenderer::AnchorPosition::TopLeft)
				anchor.y = height;
			else if (spriteDrawcall.Anchor == SpriteRenderer::AnchorPosition::TopRight)
				anchor = float2(width, height);

			// Pack the position and scale into a single float4
			spriteData.PositionScale = float4(anchor + spriteDrawcall.Position, spriteDrawcall.Scale);
			spriteData.BaseColor = spriteDrawcall.BaseColor;
			spriteData.Fill = float4(spriteDrawcall.Fill, 0.0f, 0.0f);
			spriteData.Projection = orthoProjection;

			// Update the sprite ubo
			Ref<VulkanBuffer> uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(m_SpriteDataUBO[i]);
			uniformBuffer->CopyData(sizeof(SpriteData), &spriteData);

			// Set the pipeline
			commandBuffer->BindGraphicsPipeline(m_GraphicsPipeline);

			// Push the descriptors
			m_PushDescriptors->Clear();
			m_PushDescriptors->AddBuffer(m_SpriteDataUBO[i], 0);

			if (spriteDrawcall.Sprite.IsValid())
				m_PushDescriptors->AddTexture(spriteDrawcall.Sprite, 1);

			commandBuffer->PushDescriptorsGraphics(m_PushDescriptors.Get(), m_GraphicsPipeline);

			// Bind the buffers and draw
			// TODO: Convert this into instanced rendering
			commandBuffer->BindVertexBuffer(m_QuadMesh->GetVertexBuffer());
			commandBuffer->BindIndexBuffer(m_QuadMesh->GetIndexBuffer());
			commandBuffer->DrawIndexed(m_QuadMesh->GetIndexCount(), 1, 0, 0, 0);
		}
	}

	void Opaque2DSubPass::GetAttributeDescriptions(BinaryBuffer& attributeDescriptions)
	{
		std::vector<VkVertexInputAttributeDescription> descriptions;

		// Position
		auto& positionDesc = descriptions.emplace_back();
		positionDesc.binding = 0;
		positionDesc.location = 0;
		positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionDesc.offset = offsetof(Vertex, Position);

		// Position
		auto& texCoord0Desc = descriptions.emplace_back();
		texCoord0Desc.binding = 0;
		texCoord0Desc.location = 1;
		texCoord0Desc.format = VK_FORMAT_R32G32_SFLOAT;
		texCoord0Desc.offset = offsetof(Vertex, TexCoord0);

		attributeDescriptions.WriteData(descriptions);
	}
	void Opaque2DSubPass::OnSpriteShaderModified()
	{
		if (m_GraphicsPipeline)
			ResourceManager::Destroy(m_GraphicsPipeline);

		VulkanPipelineInfo info;
		info.Shaders = m_Shader->GetResourceMap();
		info.DescriptorLayout = m_Shader->GetDescriptorLayout();
		info.BindVertexAttributeDescriptions = true;
		info.AlphaBlend = false;
		info.WriteDepth = true;
		GetAttributeDescriptions(info.AttributeDescriptions);

		m_GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);
	}
}