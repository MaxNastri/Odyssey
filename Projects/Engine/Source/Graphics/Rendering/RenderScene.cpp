#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "Texture2D.h"
#include "VulkanDescriptorPool.h"
#include "GameObject.h"
#include "VulkanShaderModule.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanGraphicsPipeline.h"

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
		descriptorLayout->AddBinding("Diffuse", DescriptorType::Sampler, ShaderStage::Fragment, 3);
		descriptorLayout->Apply();

		// Camera uniform buffer
		uint32_t cameraDataSize = sizeof(cameraData);

		for (uint32_t i = 1; i < MAX_CAMERAS; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 0, cameraDataSize);

			// Write the camera data into the ubo memory
			auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(uboID);
			uniformBuffer->AllocateMemory();
			uniformBuffer->SetMemory(cameraDataSize, &cameraData);

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
			uniformBuffer->AllocateMemory();
			uniformBuffer->SetMemory(perObjectUniformSize, &objectData);

			perObjectUniformBuffers.push_back(uboID);
		}

		// Skinning buffers
		size_t skinningBufferSize = sizeof(glm::mat4) * 128;
		std::vector<glm::mat4> baseBones;
		baseBones.resize(128);
		for (int i = 0; i < 128; i++)
		{
			baseBones[i] = glm::identity<glm::mat4>();
		}

		// Allocate the skinning buffer
		skinningBufferID = ResourceManager::Allocate<VulkanUniformBuffer>(BufferType::Uniform, 2, skinningBufferSize);

		// Write an identity matrix
		auto skinningUBO = ResourceManager::GetResource<VulkanUniformBuffer>(skinningBufferID);
		skinningUBO->AllocateMemory();
		skinningUBO->SetMemory(skinningBufferSize, baseBones.data());
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
	}

	uint32_t RenderScene::SetCameraData(Camera* camera)
	{
		cameraData.inverseView = camera->GetInverseView();
		cameraData.ViewProjection = camera->GetProjection() * cameraData.inverseView;

		uint32_t index = m_NextCameraBuffer;
		uint32_t sceneUniformSize = sizeof(cameraData);

		auto uniformBuffer = ResourceManager::GetResource<VulkanUniformBuffer>(cameraDataBuffers[m_NextCameraBuffer]);
		uniformBuffer->SetMemory(sceneUniformSize, &cameraData);

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

			setPass.SetMaterial(meshRenderer.GetMaterial(), m_DescriptorLayout);

			// Create the drawcall data
			if (auto mesh = meshRenderer.GetMesh())
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
				uniformBuffer->SetMemory(perObjectSize, &objectData);
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