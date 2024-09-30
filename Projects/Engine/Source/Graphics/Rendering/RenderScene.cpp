#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "VulkanUniformBuffer.h"
#include "Texture2D.h"
#include "VulkanDescriptorPool.h"
#include "GameObject.h"
#include "VulkanShaderModule.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanGraphicsPipeline.h"
#include "AssetManager.h"
#include "Animator.h"
#include "Cubemap.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "ParticleBatcher.h"

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
		descriptorLayout->Apply();

		// Camera uniform buffer
		uint32_t cameraDataSize = sizeof(cameraData);

		for (uint32_t i = 0; i < MAX_CAMERAS; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 0, cameraDataSize);

			// Write the camera data into the ubo memory
			auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(uboID);
			uniformBuffer->CopyData(cameraDataSize, &cameraData);

			cameraDataBuffers.push_back(uboID);
		}

		// Per-object uniform buffers
		uint32_t perObjectUniformSize = sizeof(objectData);

		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 1, perObjectUniformSize);

			// Write the per-object data into the ubo
			auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(uboID);
			uniformBuffer->CopyData(perObjectUniformSize, &objectData);

			perObjectUniformBuffers.push_back(uboID);
		}

		// Skinning buffers
		uint32_t skinningSize = sizeof(SkinningData);

		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 2, skinningSize);

			// Write the per-object data into the ubo
			auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(uboID);
			uniformBuffer->CopyData(skinningSize, &SkinningData);

			skinningBuffers.push_back(uboID);
		}

		// Skinning buffers
		uint32_t lightSize = sizeof(LightingData);

		// Allocate the UBO
		LightingBuffer = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 3, lightSize);

		// Write the per-object data into the ubo
		auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(LightingBuffer);
		uniformBuffer->CopyData(lightSize, &LightingData);
	}

	void RenderScene::Destroy()
	{
		ResourceManager::Destroy(m_DescriptorLayout);

		for (auto& resource : cameraDataBuffers)
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

		// Search the scene for the main camera
		for (auto entity : scene->GetAllEntitiesWith<Camera>())
		{
			GameObject gameObject = GameObject(scene, entity);
			Camera& camera = gameObject.GetComponent<Camera>();

			if (camera.IsMainCamera())
			{
				m_MainCamera = &camera;
				SetCameraData(m_MainCamera);
			}
		}

		EnvironmentSettings envSettings = scene->GetEnvironmentSettings();
		if (envSettings.Skybox)
		{
			auto skyboxTexture = AssetManager::LoadAsset<Cubemap>(envSettings.Skybox);
			SkyboxCubemap = skyboxTexture->GetTexture();
		}

		for (auto entity : scene->GetAllEntitiesWith<Light>())
		{
			GameObject gameObject = GameObject(scene, entity);
			Light& light = gameObject.GetComponent<Light>();

			SceneLight& sceneLight = LightingData.SceneLights[LightingData.LightCount];
			sceneLight.Type = (uint32_t)light.GetType();
			sceneLight.Position = glm::vec4(light.GetPosition(), 1.0f);
			sceneLight.Direction = glm::vec4(light.GetDirection(), 1.0f);
			sceneLight.Color = glm::vec4(light.GetColor(), 1.0f);
			sceneLight.Intensity = light.GetIntensity();
			sceneLight.Range = light.GetRange();
			LightingData.LightCount++;
		}

		// Set the ambient color from the environment settings
		LightingData.AmbientColor = glm::vec4(scene->GetEnvironmentSettings().AmbientColor, 1.0f);

		// Copy the data into the uniform buffer
		auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(LightingBuffer);
		uniformBuffer->CopyData(sizeof(LightingData), &LightingData);

		std::vector<ParticleSystem> systems;
		for (auto entity : scene->GetAllEntitiesWith<ParticleSystem>())
		{
			GameObject gameObject = GameObject(scene, entity);
			systems.push_back(gameObject.GetComponent<ParticleSystem>());
		}
		ParticleBatcher::Update(systems);

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
		cameraData.inverseView = camera->GetInverseView();
		cameraData.ViewProjection = camera->GetProjection() * cameraData.inverseView;
		cameraData.ViewPosition = glm::vec4(camera->GetView()[3][0], camera->GetView()[3][1], camera->GetView()[3][2], 1.0f);

		uint32_t index = m_NextCameraBuffer;
		uint32_t sceneUniformSize = sizeof(cameraData);

		auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(cameraDataBuffers[m_NextCameraBuffer]);
		uniformBuffer->CopyData(sceneUniformSize, &cameraData);

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

			if (!meshRenderer.GetMaterial() || !meshRenderer.GetMesh())
				continue;

			// For now, 1 set pass per drawcall
			setPasses.push_back(SetPass());
			SetPass& setPass = setPasses[setPasses.size() - 1];

			auto material = AssetManager::LoadAsset<Material>(meshRenderer.GetMaterial());
			setPass.SetMaterial(material, m_DescriptorLayout);

			// Create the drawcall data
			if (auto mesh = AssetManager::LoadAsset<Mesh>(meshRenderer.GetMesh()))
			{
				Drawcall drawcall;
				drawcall.VertexBufferID = mesh->GetVertexBuffer();
				drawcall.IndexBufferID = mesh->GetIndexBuffer();
				drawcall.IndexCount = mesh->GetIndexCount();
				drawcall.UniformBufferIndex = m_NextUniformBuffer++;
				setPass.Drawcalls.push_back(drawcall);

				// Update the per-object uniform buffer
				uint32_t perObjectSize = sizeof(objectData);
				objectData.world = transform.GetWorldMatrix();
				ResourceID uboID = perObjectUniformBuffers[drawcall.UniformBufferIndex];
				auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(uboID);
				uniformBuffer->CopyData(perObjectSize, &objectData);

				if (auto animator = gameObject.TryGetComponent<Animator>())
				{
					size_t skinningSize = sizeof(SkinningData);
					SkinningData.SetBindposes(animator->GetFinalPoses());

					ResourceID skinningID = skinningBuffers[drawcall.UniformBufferIndex];
					auto skinningBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(skinningID);
					skinningBuffer->CopyData(skinningSize, &SkinningData);
				}
			}
		}
	}

	SetPass::SetPass(std::shared_ptr<Material> material, ResourceID descriptorLayout)
	{
		SetMaterial(material, descriptorLayout);
	}

	void SetPass::SetMaterial(std::shared_ptr<Material> material, ResourceID descriptorLayout)
	{
		// Allocate a graphics pipeline
		VulkanPipelineInfo info;
		info.Shaders = material->GetShader()->GetResourceMap();
		info.DescriptorLayout = descriptorLayout;

		Shaders = info.Shaders;
		GraphicsPipeline = ResourceManager::Allocate<VulkanGraphicsPipeline>(info);

		if (auto texture = material->GetTexture())
			Texture = texture->GetTexture();
	}
}