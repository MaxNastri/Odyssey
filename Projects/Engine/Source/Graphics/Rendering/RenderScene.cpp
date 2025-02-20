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
#include "SceneManager.h"
#include "SpriteRenderer.h"

namespace Odyssey
{
	RenderScene::RenderScene()
	{
		// Camera uniform buffer
		for (uint32_t i = 0; i < MAX_CAMERAS; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(SceneData));

			// Write the camera data into the ubo memory
			SceneData sceneData;
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(sizeof(SceneData), &sceneData);

			sceneDataBuffers.push_back(uboID);
		}

		// Per-object uniform buffers
		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(ObjectUniformData));

			// Write the per-object data into the ubo
			ObjectUniformData objectData;
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(sizeof(ObjectUniformData), &objectData);

			perObjectUniformBuffers.push_back(uboID);
		}

		// Skinning buffers
		for (uint32_t i = 0; i < Max_Uniform_Buffers; i++)
		{
			// Allocate the UBO
			ResourceID uboID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(SkinningData));

			// Write the per-object data into the ubo
			SkinningData skinningData;
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(sizeof(SkinningData), &skinningData);

			skinningBuffers.push_back(uboID);
		}

		// Allocate the UBO
		LightingBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Uniform, sizeof(LightingData));

		// Write the per-object data into the ubo
		LightingData lightingData;
		auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(LightingBuffer);
		uniformBuffer->CopyData(sizeof(LightingData), &lightingData);
	}

	void RenderScene::Destroy()
	{
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

		LightingData lightingData;
		lightingData.AmbientColor = float4(scene->GetEnvironmentSettings().AmbientColor, 1.0f);
		lightingData.Exposure = scene->GetEnvironmentSettings().Exposure;
		lightingData.GammaCorrection = scene->GetEnvironmentSettings().GammaCorrection;

		for (auto entity : scene->GetAllEntitiesWith<Light>())
		{
			GameObject gameObject = GameObject(scene, entity);
			Light& light = gameObject.GetComponent<Light>();

			if (light.IsEnabled())
			{
				SceneLight& sceneLight = lightingData.SceneLights[lightingData.LightCount];
				sceneLight.Type = (uint32_t)light.GetType();
				sceneLight.Position = glm::vec4(light.GetPosition(), 1.0f);
				sceneLight.Direction = glm::vec4(light.GetDirection(), 1.0f);
				sceneLight.Color = glm::vec4(light.GetColor(), 1.0f);
				sceneLight.Intensity = light.GetIntensity();
				sceneLight.Range = light.GetRange();
				lightingData.LightCount++;

				if (light.GetType() == LightType::Directional)
				{
					m_ShadowLight = &light;
				}
			}
		}

		// Cache the shadow light view projection matrix
		if (m_ShadowLight)
			m_ShadowLightMatrix = Light::CalculateViewProj(envSettings.SceneCenter, envSettings.SceneRadius, m_ShadowLight->GetDirection());

		// Update the lighting ubo
		auto lightingUBO = ResourceManager::GetResource<VulkanBuffer>(LightingBuffer);
		lightingUBO->CopyData(sizeof(LightingData), &lightingData);

		// Search the scene for the main camera
		for (auto entity : scene->GetAllEntitiesWith<Camera>())
		{
			// Get the camera component
			GameObject gameObject = GameObject(scene, entity);
			Camera* camera = gameObject.TryGetComponent<Camera>();

			// Cache the cameras for lookup later
			uint8_t cameraTag = (uint8_t)camera->GetTag();
			m_Cameras[cameraTag] = camera;
		}

		ParticleBatcher::Update();
		SetupDrawcalls(scene);
	}

	void RenderScene::ClearSceneData()
	{
		SetPasses.clear();
		SpriteDrawcalls.clear();
		m_GUIDToSetPass.clear();
		m_NextUniformBuffer = 0;
		m_MainCamera = nullptr;
	}

	void RenderScene::SetSceneData(uint8_t cameraTag)
	{
		if (m_Cameras.contains(cameraTag))
		{
			Camera* camera = m_Cameras[cameraTag];

			// TODO: Fix this to not use the scene manager
			EnvironmentSettings envSettings = SceneManager::GetActiveScene()->GetEnvironmentSettings();

			SceneData sceneData;
			sceneData.View = camera->GetInverseView();
			sceneData.Projection = camera->GetProjection();
			sceneData.ViewProjection = sceneData.Projection * sceneData.View;
			sceneData.ViewPosition = camera->GetViewPosition();

			if (m_ShadowLight)
				sceneData.LightViewProj = m_ShadowLightMatrix;

			// Update the scene ubo
			Ref<VulkanBuffer> sceneUBO = ResourceManager::GetResource<VulkanBuffer>(sceneDataBuffers[cameraTag]);
			sceneUBO->CopyData(sizeof(sceneData), &sceneData);
		}
	}

	Camera* RenderScene::GetCamera(uint8_t cameraTag)
	{
		if (m_Cameras.contains(cameraTag))
			return m_Cameras[cameraTag];

		return nullptr;
	}

	void RenderScene::SetupDrawcalls(Scene* scene)
	{
		for (auto entity : scene->GetAllEntitiesWith<MeshRenderer, Transform>())
		{
			GameObject gameObject = GameObject(scene, entity);
			MeshRenderer& meshRenderer = gameObject.GetComponent<MeshRenderer>();
			Transform& transform = gameObject.GetComponent<Transform>();
			Animator* animator = gameObject.TryGetComponent<Animator>();

			std::vector<Ref<Material>>& materials = meshRenderer.GetMaterials();
			Ref<Mesh> mesh = meshRenderer.GetMesh();

			if (!meshRenderer.IsEnabled() || materials.size() == 0 || !mesh)
				continue;

			uint32_t uboIndex = m_NextUniformBuffer++;

			for (size_t i = 0; i < materials.size(); i++)
			{
				if (!materials[i] || materials[i]->GetGUID() == 0)
					continue;

				RenderQueue renderQueue = materials[i]->GetRenderQueue();
				std::vector<SetPass>& setPasses = SetPasses[renderQueue];

				if (SubMesh* submesh = mesh->GetSubmesh(i))
				{
					GUID materialGUID = materials[i]->GetGUID();
					SetPass* setPass = nullptr;

					if (m_GUIDToSetPass.contains(materialGUID))
					{
						size_t index = m_GUIDToSetPass[materialGUID];
						setPass = &setPasses[index];
					}
					else
					{
						size_t index = setPasses.size();
						setPass = &setPasses.emplace_back();

						m_GUIDToSetPass[materialGUID] = index;

						setPass->SetMaterial(materials[i], animator != nullptr);
					}

					// Create the drawcall data
					Drawcall& drawcall = setPass->Drawcalls.emplace_back();
					drawcall.VertexBufferID = submesh->VertexBuffer;
					drawcall.IndexBufferID = submesh->IndexBuffer;
					drawcall.IndexCount = submesh->IndexCount;
					drawcall.UniformBufferIndex = uboIndex;
					drawcall.Skinned = animator != nullptr;
				}
			}

			// Update the per-object uniform buffer
			ObjectUniformData objectData;
			uint32_t perObjectSize = sizeof(objectData);
			objectData.world = transform.GetWorldMatrix();

			ResourceID uboID = perObjectUniformBuffers[uboIndex];
			auto uniformBuffer = ResourceManager::GetResource<VulkanBuffer>(uboID);
			uniformBuffer->CopyData(perObjectSize, &objectData);

			if (animator)
			{
				// Update the skinning buffer
				SkinningData skinningData;
				skinningData.SetBindposes(animator->GetFinalPoses());

				ResourceID skinningID = skinningBuffers[uboIndex];
				auto skinningBuffer = ResourceManager::GetResource<VulkanBuffer>(skinningID);
				skinningBuffer->CopyData(sizeof(SkinningData), &skinningData);
			}
		}

		for (auto entity : scene->GetAllEntitiesWith<SpriteRenderer, Transform>())
		{
			GameObject gameObject = GameObject(scene, entity);
			SpriteRenderer& spriteRenderer = gameObject.GetComponent<SpriteRenderer>();
			Transform& transform = gameObject.GetComponent<Transform>();

			if (spriteRenderer.IsEnabled())
			{
				SpriteDrawcall& drawcall = SpriteDrawcalls.emplace_back();
				drawcall.Anchor = spriteRenderer.GetAnchor();
				drawcall.Position = transform.GetPosition();
				drawcall.Scale = transform.GetScale();
				drawcall.Fill = spriteRenderer.GetFill();
				drawcall.BaseColor = spriteRenderer.GetBaseColor();

				if (spriteRenderer.GetSprite())
					drawcall.Sprite = spriteRenderer.GetSprite()->GetTexture();
			}
		}
	}

	void SetPass::SetMaterial(Ref<Material> material, bool skinned)
	{
		Shaders = material->GetShader()->GetResourceMap();
		GraphicsPipeline = material->GetPipeline();

		// Textures
		ShaderBindings = material->GetShader()->GetBindings();
		Textures = material->GetTextures();

		// Store the material buffer for binding
		MaterialBuffer = material->GetMaterialBuffer();

		// Store the render queue
		RenderQueue = material->GetRenderQueue();

		WriteDepth = material->GetDepthWrite();
	}
}