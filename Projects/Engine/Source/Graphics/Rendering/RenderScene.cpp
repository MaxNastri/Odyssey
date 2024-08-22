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

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Descriptor layout for the combined uniform buffers
		uboLayout = ResourceManager::AllocateDescriptorLayout(DescriptorType::Uniform, ShaderStage::Vertex, 0);
		m_Layouts.push_back(uboLayout);

		// Camera uniform buffer
		uint32_t cameraDataSize = sizeof(cameraData);

		for (uint32_t i = 1; i < MAX_CAMERAS; i++)
		{
			// Allocate the UBO
			ResourceHandle<VulkanUniformBuffer> uniformBuffer = ResourceManager::AllocateUniformBuffer(BufferType::Uniform, 0, cameraDataSize);
			uniformBuffer.Get()->AllocateMemory();
			uniformBuffer.Get()->SetMemory(cameraDataSize, &cameraData);
			cameraDataBuffers.push_back(uniformBuffer);
		}

		// Per-object uniform buffers
		uint32_t perObjectUniformSize = sizeof(objectData);

		for (uint32_t i = 1; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceHandle<VulkanUniformBuffer> uniformBuffer = ResourceManager::AllocateUniformBuffer(BufferType::Uniform, 1, perObjectUniformSize);
			uniformBuffer.Get()->AllocateMemory();
			uniformBuffer.Get()->SetMemory(perObjectUniformSize, &objectData);
			perObjectUniformBuffers.push_back(uniformBuffer);
		}
	}

	void RenderScene::Destroy()
	{
		ResourceManager::DestroyDescriptorLayout(uboLayout);
		for (auto& buffer : cameraDataBuffers)
		{
			ResourceManager::DestroyUniformBuffer(buffer);
		}
		for (auto& buffer : perObjectUniformBuffers)
		{
			ResourceManager::DestroyUniformBuffer(buffer);
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
			ResourceManager::DestroyGraphicsPipeline(setPass.pipeline);
		}

		setPasses.clear();
		m_NextUniformBuffer = 0;
		m_NextCameraBuffer = 0;
		m_MainCamera = nullptr;
	}

	uint32_t RenderScene::SetCameraData(Camera* camera)
	{
		cameraData.inverseView = camera->GetInverseView();
		cameraData.proj = camera->GetProjection();

		uint32_t index = m_NextCameraBuffer;
		uint32_t sceneUniformSize = sizeof(cameraData);
		cameraDataBuffers[m_NextCameraBuffer]->SetMemory(sceneUniformSize, &cameraData);
		
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

			// For now, 1 set pass per drawcall
			setPasses.push_back(SetPass());
			SetPass& setPass = setPasses[setPasses.size() - 1];

			setPass.SetMaterial(meshRenderer.GetMaterial(), m_Layouts);

			// Create the drawcall data
			if (auto mesh = meshRenderer.GetMesh())
			{
				Drawcall drawcall;
				drawcall.VertexBuffer = mesh->GetVertexBuffer();
				drawcall.IndexBuffer = mesh->GetIndexBuffer();
				drawcall.IndexCount = mesh->GetIndexCount();
				drawcall.UniformBufferIndex = m_NextUniformBuffer++;
				setPass.drawcalls.push_back(drawcall);

				// Update the per-object uniform buffer
				uint32_t perObjectSize = sizeof(objectData);
				objectData.world = transform.GetWorldMatrix();
				perObjectUniformBuffers[drawcall.UniformBufferIndex].Get()->SetMemory(perObjectSize, &objectData);
			}
		}
	}

	SetPass::SetPass(std::shared_ptr<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts)
	{
		SetMaterial(material, descriptorLayouts);
	}

	void SetPass::SetMaterial(std::shared_ptr<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts)
	{
		// Allocate a graphics pipeline
		VulkanPipelineInfo info;
		info.vertexShader = material->GetVertexShader()->GetShaderModule();
		info.fragmentShader = material->GetFragmentShader()->GetShaderModule();
		info.descriptorLayouts = descriptorLayouts;
		pipeline = ResourceManager::AllocateGraphicsPipeline(info);

		// Store the IDs
		vertexShaderID = info.vertexShader.GetID();
		fragmentShaderID = info.fragmentShader.GetID();

		if (auto texture = material->GetTexture())
		{
			Texture = texture->GetTexture();
		}
	}
}