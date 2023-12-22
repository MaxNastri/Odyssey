#pragma once
#include "glm.h"
#include "ResourceHandle.h"
#include "AssetHandle.h"
#include "Drawcall.h"

namespace Odyssey
{
	class Camera;
	class Material;
	class Mesh;
	class Scene;
	class VulkanGraphicsPipeline;
	class VulkanDescriptorBuffer;
	class VulkanBuffer;
	class VulkanDescriptorLayout;
	class VulkanImage;
	class VulkanTextureSampler;

	struct SceneUniformData
	{
		glm::mat4 inverseView;
		glm::mat4 proj;
	};

	struct ObjectUniformData
	{
		glm::mat4 world;
	};

	struct SetPass
	{
	public:
		SetPass() = default;
		SetPass(AssetHandle<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts);

	public:
		void SetMaterial(AssetHandle<Material> material, std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts);

	public:
		ResourceHandle<VulkanGraphicsPipeline> pipeline;
		std::vector<Drawcall> drawcalls;
		int32_t vertexShaderID = -1;
		int32_t fragmentShaderID = -1;
		ResourceHandle<VulkanImage> Texture;
		ResourceHandle<VulkanTextureSampler> Sampler;
	};

	class RenderScene
	{
	public:
		RenderScene();
		void Destroy();

	public:
		void ConvertScene(Scene* scene);
		void ClearSceneData();

		void SetCameraData(Camera* camera);

	private:
		void SetupDrawcalls(Scene* scene);

	public:
		// Data structs
		SceneUniformData sceneData;
		ObjectUniformData objectData;

		// Descriptor buffer for per-scene data
		ResourceHandle<VulkanDescriptorLayout> uboLayout;
		ResourceHandle<VulkanBuffer> sceneUniformBuffer;
		std::vector<ResourceHandle<VulkanBuffer>> perObjectUniformBuffers;
		ResourceHandle<VulkanDescriptorBuffer> descriptorBuffer;

		// Descriptor buffer for per-object sampler
		ResourceHandle<VulkanDescriptorLayout> m_SamplerLayout;
		ResourceHandle<VulkanBuffer> m_SamplerBuffer;
		ResourceHandle<VulkanDescriptorBuffer> m_SamplerDescriptorBuffer;

		std::vector<ResourceHandle<VulkanDescriptorLayout>> m_Layouts;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
	};
}