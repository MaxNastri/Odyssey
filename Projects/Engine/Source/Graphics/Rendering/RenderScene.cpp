#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "VulkanShaderModule.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanGraphicsPipeline.h"
#include "AssetManager.h"
#include "Animator.h"
#include "Cubemap.h"
#include "Light.h"
#include "ParticleBatcher.h"
#include "VulkanBuffer.h"

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Descriptor layout for the combined uniform buffers
		m_DescriptorLayout = ResourceManager::Allocate<VulkanDescriptorLayout>();

		auto descriptorLayout = ResourceManager::GetResource<VulkanDescriptorLayout>(m_DescriptorLayout);
		descriptorLayout->AddBinding("Scene Data", DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorLayout->AddBinding("Model Data", DescriptorType::Uniform, ShaderStage::Vertex, 1);
		descriptorLayout->AddBinding("Skinning Data", DescriptorType::Uniform, ShaderStage::Vertex, 2);
		descriptorLayout->AddBinding("Lighting Data", DescriptorType::Uniform, ShaderStage::Fragment, 3);
		descriptorLayout->AddBinding("Diffuse", DescriptorType::Sampler, ShaderStage::Fragment, 4);
		descriptorLayout->AddBinding("Normal", DescriptorType::Sampler, ShaderStage::Fragment, 5);
		descriptorLayout->AddBinding("Shadowmap", DescriptorType::Sampler, ShaderStage::Fragment, 6);
		descriptorLayout->Apply();

		// Camera uniform buffer
		uint32_t cameraDataSize = sizeof(sceneData);

		for (uint32_t i = 0; i < MAX_CAMERAS; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, cameraDataSize);

			// Write the camera data into the ubo memory
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(cameraDataSize, &sceneData);

			sceneDataBuffers.push_back(uboID);
		}

		// Per-object uniform buffers
		uint32_t perObjectUniformSize = sizeof(objectData);

		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, perObjectUniformSize);

			// Write the per-object data into the ubo
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(perObjectUniformSize, &objectData);

			perObjectUniformBuffers.push_back(uboID);
		}

		// Skinning buffers
		uint32_t skinningSize = sizeof(SkinningData);

		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, skinningSize);

			// Write the per-object data into the ubo
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(skinningSize, &SkinningData);

			skinningBuffers.push_back(uboID);
		}

		// Skinning buffers
		uint32_t lightSize = sizeof(LightingData);

		// Allocate the UBO
		LightingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, lightSize);

		// Write the per-object data into the ubo
		auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(LightingBuffer);
		uniformBuffer->CopyData(lightSize, &LightingData);
	}

	void RenderScene::Destroy()
	{
		ResourceManager::Destroy(m_DescriptorLayout);

		for (auto& resource : sceneDataBuffers)
		{
			ResourceManager::Destroy(resource);
		}

		for (auto& resource : perObjectUniformBuffers)
		{
			ResourceManager::Destroy(resource);
		}
	}

	void RenderScene::ConvertScene(Scene* scene)
	{
		ClearSceneData();

		EnvironmentSettings envSettings = scene->GetEnvironmentSettings();
		if (envSettings.Skybox)
			SkyboxCubemap = envSettings.Skybox->GetTexture();
		else
			SkyboxCubemap = ResourceID::Invalid();

		for (auto entity : scene->GetAllEntitiesWith<Light>())
		{
			GameObject gameObject = GameObject(scene, entity);
			Light& light = gameObject.GetComponent<Light>();

			if (light.IsEnabled())
			{
				SceneLight& sceneLight = LightingData.SceneLights[LightingData.LightCount];
				sceneLight.Type = (uint32_t)light.GetType();
				sceneLight.Position = glm::vec4(light.GetPosition(), 1.0f);
				sceneLight.Direction = glm::vec4(light.GetDirection(), 1.0f);
				sceneLight.Color = glm::vec4(light.GetColor(), 1.0f);
				sceneLight.Intensity = light.GetIntensity();
				sceneLight.Range = light.GetRange();
				LightingData.LightCount++;

				if (light.GetType() == LightType::Directional)
					sceneData.LightViewProj = Light::CalculateViewProj(float3(0.0f), 10.0f, sceneLight.Position, sceneLight.Direction);
			}
		}

		// Search the scene for the main camera
		for (auto entity : scene->GetAllEntitiesWith<Camera>())
		{
			GameObject gameObject = GameObject(scene, entity);
			Camera& camera = gameObject.GetComponent<Camera>();

			if (camera.IsEnabled() && camera.IsMainCamera())
			{
				m_MainCamera = &camera;
				SetCameraData(m_MainCamera);
			}
		}

		// Set the ambient color from the environment settings
		LightingData.AmbientColor = glm::vec4(scene->GetEnvironmentSettings().AmbientColor, 1.0f);


		// Update the lighting ubo
		auto lightingUBO = ResourceManager::GetResource<VulkanBuffer>(LightingBuffer);
		lightingUBO->CopyData(sizeof(LightingData), &LightingData);

		ParticleBatcher::Update();

		SetupDrawcalls(scene);
	}

	void RenderScene::ClearSceneData()
	{
		for (auto& setPass : setPasses)
		{
			ResourceManager::Destroy(setPass.GraphicsPipeline);
		}

		setPasses.clear();
		m_NextUniformBuffer = 0;
		m_NextCameraBuffer = 0;
		m_MainCamera = nullptr;
		LightingData.LightCount = 0;
	}

	uint32_t RenderScene::SetCameraData(Camera* camera)
	{
		sceneData.View = camera->GetInverseView();
		sceneData.ViewProjection = camera->GetProjection() * camera->GetInverseView();
		sceneData.ViewPosition = glm::vec4(camera->GetView()[3][0], camera->GetView()[3][1], camera->GetView()[3][2], 1.0f);

		uint32_t index = m_NextCameraBuffer;

		// Update the scene ubo
		auto sceneUBO = ResourceManager::GetResource<VulkanBuffer>(sceneDataBuffers[index]);
		sceneUBO->CopyData(sizeof(sceneData), &sceneData);

		m_NextCameraBuffer++;
		return index;
	}

	void RenderScene::SetupDrawcalls(Scene* scene)
	{
		for (auto entity : scene->GetAllEntitiesWith<MeshRenderer, Transform>())
		{
			GameObject gameObject = GameObject(scene, entity);
			MeshRenderer& meshRenderer = gameObject.GetComponent<MeshRenderer>();
			Transform& transform = gameObject.GetComponent<Transform>();
			Animator* animator = gameObject.TryGetComponent<Animator>();

			const std::vector<Ref<Material>>& materials = meshRenderer.GetMaterials();
			Ref<Mesh> mesh = meshRenderer.GetMesh();

			if (!meshRenderer.IsEnabled() || materials.size() == 0 || !mesh)
				continue;

			uint32_t uboIndex = m_NextUniformBuffer++;

			for (size_t i = 0; i < materials.size(); i++)
			{
				if (!materials[i])
					continue;

				if (SubMesh* submesh = mesh->GetSubmesh(i))
				{
					// For now, 1 set pass per drawcall
					SetPass& setPass = setPasses.emplace_back();
					setPass.SetMaterial(materials[i], animator != nullptr, m_DescriptorLayout);

					// Create the drawcall data
					Drawcall& drawcall = setPass.Drawcalls.emplace_back();
					drawcall.VertexBufferID = submesh->VertexBuffer;
					drawcall.IndexBufferID = submesh->IndexBuffer;
					drawcall.IndexCount = submesh->IndexCount;
					drawcall.UniformBufferIndex = uboIndex;
					drawcall.Skinned = animator != nullptr;
				}
			}

			// Update the per-object uniform buffer
			uint32_t perObjectSize = sizeof(objectData);
			objectData.world = transform.GetWorldMatrix();
			objectData.InverseWorld = glm::transpose(glm::inverse(objectData.world));

			ResourceID uboID = perObjectUniformBuffers[uboIndex];
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(perObjectSize, &objectData);

			if (animator)
			{
				// Update the skinning buffer
				size_t skinningSize = sizeof(SkinningData);
				SkinningData.SetBindposes(animator->GetFinalPoses());

				ResourceID skinningID = skinningBuffers[uboIndex];
				auto skinningBuffer = ResourceManager::GetResource<VulkanBuffer>(skinningID);
				skinningBuffer->CopyData(skinningSize, &SkinningData);
			}
		}
	}

	SetPass::SetPass(Ref<Material> material, bool skinned, ResourceID descriptorLayout)
	{
		SetMaterial(material, skinned, descriptorLayout);
	}

	void SetPass::SetMaterial(Ref<Material> material, bool skinned, ResourceID descriptorLayout)
	{
		// Allocate a graphics pipeline
		VulkanPipelineInfo info;
		Shaders = info.Shaders = material->GetShader()->GetResourceMap();
		info.DescriptorLayout = descriptorLayout;
		info.CullMode = CullMode::Back;
		SetupAttributeDescriptions(skinned, info.AttributeDescriptions);

		GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);

		if (Ref<Texture2D> colorTexture = material->GetColorTexture())
			ColorTexture = colorTexture->GetTexture();

		if (Ref<Texture2D> normalTexture = material->GetNormalTexture())
			NormalTexture = normalTexture->GetTexture();
	}

	void SetPass::SetupAttributeDescriptions(bool skinned, BinaryBuffer& descriptions)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		// Position
		auto& positionDesc = attributeDescriptions.emplace_back();
		positionDesc.binding = 0;
		positionDesc.location = 0;
		positionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionDesc.offset = offsetof(Vertex, Position);

		// Normal
		auto& normalDesc = attributeDescriptions.emplace_back();
		normalDesc.binding = 0;
		normalDesc.location = 1;
		normalDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalDesc.offset = offsetof(Vertex, Normal);

		// Tangent
		auto& tangentDesc = attributeDescriptions.emplace_back();
		tangentDesc.binding = 0;
		tangentDesc.location = 2;
		tangentDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		tangentDesc.offset = offsetof(Vertex, Tangent);

		// TexCoord0
		auto& texCoord0Desc = attributeDescriptions.emplace_back();
		texCoord0Desc.binding = 0;
		texCoord0Desc.location = 3;
		texCoord0Desc.format = VK_FORMAT_R32G32_SFLOAT;
		texCoord0Desc.offset = offsetof(Vertex, TexCoord0);

		if (skinned)
		{
			// Bone Indices
			auto& indicesDesc = attributeDescriptions.emplace_back();
			indicesDesc.binding = 0;
			indicesDesc.location = 4;
			indicesDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			indicesDesc.offset = offsetof(Vertex, BoneIndices);

			// Bone Weights
			auto& weightsDesc = attributeDescriptions.emplace_back();
			weightsDesc.binding = 0;
			weightsDesc.location = 5;
			weightsDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			weightsDesc.offset = offsetof(Vertex, BoneWeights);
		}

		descriptions.WriteData(attributeDescriptions);
	}
}