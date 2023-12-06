#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorBuffer.h"

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Descriptor layout for the combined uniform buffers
		descriptorLayout = ResourceManager::AllocateDescriptorLayout(DescriptorType::Uniform, ShaderStage::Vertex, 0);
		descriptorBuffer = ResourceManager::AllocateDescriptorBuffer(descriptorLayout, Max_Uniform_Buffers);

		// Scene uniform buffer
		uint32_t sceneUniformSize = sizeof(sceneData);
		sceneUniformBuffer = ResourceManager::AllocateBuffer(BufferType::Uniform, sceneUniformSize);
		sceneUniformBuffer.Get()->AllocateMemory();
		sceneUniformBuffer.Get()->SetMemory(sceneUniformSize, &sceneData);

		// Put the scene uniform buffer into the descriptor buffer [0]
		descriptorBuffer.Get()->SetUniformBuffer(sceneUniformBuffer, 0);

		// Per-object uniform buffers
		uint32_t perObjectUniformSize = sizeof(objectData);

		for (uint32_t i = 1; i < Max_Uniform_Buffers; i++)
		{
			ResourceHandle<VulkanBuffer> uniformBuffer = ResourceManager::AllocateBuffer(BufferType::Uniform, perObjectUniformSize);
			uniformBuffer.Get()->AllocateMemory();
			uniformBuffer.Get()->SetMemory(perObjectUniformSize, &objectData);
			perObjectUniformBuffers.push_back(uniformBuffer);

			// Put the per-object uniform buffer into the descriptor buffer [i]
			descriptorBuffer.Get()->SetUniformBuffer(uniformBuffer, i);
		}
	}

	void RenderScene::Destroy()
	{
		ResourceManager::DestroyDescriptorLayout(descriptorLayout);
		ResourceManager::DestroyDescriptorBuffer(descriptorBuffer);
		ResourceManager::DestroyBuffer(sceneUniformBuffer);
	}

	void RenderScene::ConvertScene(Scene* scene)
	{
		ClearSceneData();
		SetupCameraData(scene);
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

	void RenderScene::SetupCameraData(Scene* scene)
	{
		if (Camera* mainCamera = scene->GetMainCamera())
		{
			sceneData.inverseView = mainCamera->GetInverseView();
			sceneData.proj = mainCamera->GetProjection();

			uint32_t sceneUniformSize = sizeof(sceneData);
			sceneUniformBuffer.Get()->SetMemory(sceneUniformSize, &sceneData);
		}
	}

	void RenderScene::SetupDrawcalls(Scene* scene)
	{
		for (auto& gameObject : scene->gameObjects)
		{
			if (MeshRenderer* renderer = ComponentManager::GetComponent<MeshRenderer>(gameObject->id))
			{
				if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject->id))
				{
					SetPass* setPass = nullptr;
					Drawcall drawcall;

					// Create the set pass, if its not already created
					if (!SetPassCreated(renderer->GetMaterial(), setPass))
					{
						setPasses.push_back(SetPass());
						setPass = &(setPasses[setPasses.size() - 1]);
						setPass->SetMaterial(renderer->GetMaterial(), descriptorLayout);
					}

					Mesh* mesh = renderer->GetMesh().Get();

					// Create the drawcall data
					drawcall.VertexBuffer = mesh->GetVertexBuffer();
					drawcall.IndexBuffer = mesh->GetIndexBuffer();
					drawcall.IndexCount = mesh->GetIndexCount();
					drawcall.UniformBufferIndex = m_NextUniformBuffer++;
					setPass->drawcalls.push_back(drawcall);

					// Update the per-object uniform buffer
					uint32_t perObjectSize = sizeof(objectData);
					objectData.world = transform->GetWorldMatrix();
					perObjectUniformBuffers[drawcall.UniformBufferIndex].Get()->SetMemory(perObjectSize, &objectData);
				}
			}
		}
	}

	bool RenderScene::SetPassCreated(AssetHandle<Material> material, SetPass* outSetPass)
	{
		for (auto& setpass : setPasses)
		{
			Material* mat = material.Get();

			if (setpass.vertexShaderID == mat->GetVertexShader().GetID() &&
				setpass.fragmentShaderID == mat->GetFragmentShader().GetID())
			{
				outSetPass = &setpass;
				return true;
			}
		}

		return false;
	}

	SetPass::SetPass(AssetHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout)
	{
		SetMaterial(material, descriptorLayout);
	}

	void SetPass::SetMaterial(AssetHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout)
	{
		// Allocate a graphics pipeline
		std::vector<ResourceHandle<VulkanDescriptorLayout>> layouts{};
		layouts.push_back(descriptorLayout);
		layouts.push_back(descriptorLayout);

		VulkanPipelineInfo info;
		info.vertexShader = material.Get()->GetVertexShader().Get()->GetShaderModule();
		info.fragmentShader = material.Get()->GetFragmentShader().Get()->GetShaderModule();
		info.descriptorLayouts = layouts;
		pipeline = ResourceManager::AllocateGraphicsPipeline(info);

		// Store the IDs
		vertexShaderID = info.vertexShader.GetID();
		fragmentShaderID = info.fragmentShader.GetID();
	}
}