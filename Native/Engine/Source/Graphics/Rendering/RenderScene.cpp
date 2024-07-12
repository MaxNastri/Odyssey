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
#include "VulkanBuffer.h"
#include "VulkanDescriptorBuffer.h"
#include "Texture2D.h"
#include "VulkanDescriptorPool.h"

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Descriptor layout for the combined uniform buffers
		uboLayout = ResourceManager::AllocateDescriptorLayout(DescriptorType::Uniform, ShaderStage::Vertex, 0);
		//m_SamplerLayout = ResourceManager::AllocateDescriptorLayout(DescriptorType::Sampler, ShaderStage::Fragment, 0);

		// Pushback 2 ubo layouts (1 scene, 1 per-object) and 1 sampler layout
		m_Layouts.push_back(uboLayout);
		//m_Layouts.push_back(m_SamplerLayout);

		// Scene uniform buffer
		uint32_t sceneUniformSize = sizeof(sceneData);
		sceneUniformBuffer = ResourceManager::AllocateBuffer(BufferType::Uniform, sceneUniformSize);
		sceneUniformBuffer.Get()->AllocateMemory();
		sceneUniformBuffer.Get()->SetMemory(sceneUniformSize, &sceneData);

		// Per-object uniform buffers
		uint32_t perObjectUniformSize = sizeof(objectData);

		for (uint32_t i = 1; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceHandle<VulkanBuffer> uniformBuffer = ResourceManager::AllocateBuffer(BufferType::Uniform, perObjectUniformSize);
			uniformBuffer.Get()->AllocateMemory();
			uniformBuffer.Get()->SetMemory(perObjectUniformSize, &objectData);
			perObjectUniformBuffers.push_back(uniformBuffer);
		}
	}

	void RenderScene::Destroy()
	{
		ResourceManager::DestroyDescriptorLayout(uboLayout);
		ResourceManager::DestroyBuffer(sceneUniformBuffer);
	}

	void RenderScene::ConvertScene(Scene* scene)
	{
		ClearSceneData();
		if (Camera* mainCamera = scene->GetMainCamera())
		{
			SetCameraData(mainCamera);
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
	}

	void RenderScene::SetCameraData(Camera* camera)
	{
		sceneData.inverseView = camera->GetInverseView();
		sceneData.proj = camera->GetProjection();

		uint32_t sceneUniformSize = sizeof(sceneData);
		sceneUniformBuffer.Get()->SetMemory(sceneUniformSize, &sceneData);
	}

	void RenderScene::SetupDrawcalls(Scene* scene)
	{
		for (auto& gameObject : scene->gameObjects)
		{
			if (MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>())
			{
				if (Transform* transform = gameObject->GetComponent<Transform>())
				{
					// For now, 1 set pass per drawcall
					setPasses.push_back(SetPass());
					SetPass& setPass = setPasses[setPasses.size() - 1];

					setPass.SetMaterial(renderer->GetMaterial(), m_Layouts);

					// Create the drawcall data
					if (Mesh* mesh = renderer->GetMesh().Get())
					{
						Drawcall drawcall;
						drawcall.VertexBuffer = mesh->GetVertexBuffer();
						drawcall.IndexBuffer = mesh->GetIndexBuffer();
						drawcall.IndexCount = mesh->GetIndexCount();
						drawcall.UniformBufferIndex = m_NextUniformBuffer++;
						setPass.drawcalls.push_back(drawcall);

						// Update the per-object uniform buffer
						uint32_t perObjectSize = sizeof(objectData);
						objectData.world = transform->GetWorldMatrix();
						perObjectUniformBuffers[drawcall.UniformBufferIndex].Get()->SetMemory(perObjectSize, &objectData);
					}
				}
			}
		}
	}

	SetPass::SetPass(AssetHandle<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts)
	{
		SetMaterial(material, descriptorLayouts);
	}

	void SetPass::SetMaterial(AssetHandle<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts)
	{
		// Allocate a graphics pipeline
		VulkanPipelineInfo info;
		info.vertexShader = material.Get()->GetVertexShader().Get()->GetShaderModule();
		info.fragmentShader = material.Get()->GetFragmentShader().Get()->GetShaderModule();
		info.descriptorLayouts = descriptorLayouts;
		pipeline = ResourceManager::AllocateGraphicsPipeline(info);

		// Store the IDs
		vertexShaderID = info.vertexShader.GetID();
		fragmentShaderID = info.fragmentShader.GetID();

		if (material.Get()->GetTexture().IsValid())
		{
			Texture = material.Get()->GetTexture().Get()->GetImage();
			Sampler = material.Get()->GetTexture().Get()->GetSampler();
		}
	}
}