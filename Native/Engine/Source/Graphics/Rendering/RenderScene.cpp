#include "RenderScene.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentManager.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceManager.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorBuffer.h"

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Uniform buffer
		uint32_t bufferSize = sizeof(uboData);
		sceneBuffer = ResourceManager::AllocateBuffer(BufferType::Uniform, bufferSize);
		sceneBuffer.Get()->AllocateMemory();
		sceneBuffer.Get()->SetMemory(bufferSize, &uboData);

		// Descriptor layout and buffer
		descriptorLayout = ResourceManager::AllocateDescriptorLayout(DescriptorType::Uniform, ShaderStage::Vertex, 0);
		sceneDescriptorBuffers.push_back(ResourceManager::AllocateDescriptorBuffer(descriptorLayout, 1));

		// Assign the uniform buffer to the descriptor buffer
		sceneDescriptorBuffers[0].Get()->SetUniformBuffer(sceneBuffer, 0);
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
	}

	void RenderScene::SetupCameraData(Scene* scene)
	{
		if (Camera* mainCamera = scene->GetMainCamera())
		{
			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			//uboData.inverseView = mainCamera->GetInverseView();
			uboData.world = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 1.0f));
			uboData.inverseView = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//uboData.proj = mainCamera->GetProjection();
			uboData.proj = glm::perspectiveLH(glm::radians(45.0f), 1000.0f / 1000.0f, 0.1f, 10.0f);
			uboData.proj[1][1] = -uboData.proj[1][1];

			uint32_t bufferSize = sizeof(uboData);
			sceneBuffer.Get()->SetMemory(bufferSize, &uboData);
		}
	}

	void RenderScene::SetupDrawcalls(Scene* scene)
	{
		for (auto& gameObject : scene->gameObjects)
		{
			if (MeshRenderer* renderer = ComponentManager::GetComponent<MeshRenderer>(gameObject))
			{
				if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
				{
					SetPass* setPass = nullptr;
					Drawcall drawcall;

					if (!SetPassCreated(renderer->GetMaterial(), setPass))
					{
						setPasses.push_back(SetPass());
						setPass = &(setPasses[setPasses.size() - 1]);
						setPass->SetMaterial(renderer->GetMaterial(), descriptorLayout);
					}

					Mesh* mesh = renderer->GetMesh().Get();
					// This is temporary, move this to a separate per-object ubo
					drawcall.VertexBuffer = mesh->GetVertexBuffer();
					drawcall.IndexBuffer = mesh->GetIndexBuffer();
					drawcall.IndexCount = mesh->GetIndexCount();

					setPass->drawcalls.push_back(drawcall);
				}
			}
		}
	}

	bool RenderScene::SetPassCreated(ResourceHandle<Material> material, SetPass* outSetPass)
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

	SetPass::SetPass(ResourceHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout)
	{
		SetMaterial(material, descriptorLayout);
	}

	void SetPass::SetMaterial(ResourceHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout)
	{
		// Allocate a graphics pipeline
		std::vector<ResourceHandle<VulkanDescriptorLayout>> layouts{};
		layouts.push_back(descriptorLayout);

		VulkanPipelineInfo info;
		info.vertexShader = material.Get()->GetVertexShader();
		info.fragmentShader = material.Get()->GetFragmentShader();
		info.descriptorLayouts = layouts;
		pipeline = ResourceManager::AllocateGraphicsPipeline(info);

		// Store the IDs
		vertexShaderID = info.vertexShader.GetID();
		fragmentShaderID = info.fragmentShader.GetID();
	}
}