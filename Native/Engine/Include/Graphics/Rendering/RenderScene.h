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
	class VulkanUniformBuffer;
	class VulkanDescriptorLayout;
	class VulkanTexture;

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
		ResourceHandle<VulkanTexture> Texture;
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
		bool HasMainCamera() { return m_MainCameraSet; }

	private:
		void SetupDrawcalls(Scene* scene);

	public:
		// Data structs
		SceneUniformData sceneData;
		ObjectUniformData objectData;
		bool m_MainCameraSet = false;

		// Descriptor buffer for per-scene data
		ResourceHandle<VulkanDescriptorLayout> uboLayout;
		ResourceHandle<VulkanUniformBuffer> sceneUniformBuffer;
		std::vector<ResourceHandle<VulkanUniformBuffer>> perObjectUniformBuffers;

		// Descriptor buffer for per-object sampler
		ResourceHandle<VulkanDescriptorLayout> m_SamplerLayout;

		std::vector<ResourceHandle<VulkanDescriptorLayout>> m_Layouts;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
	};
}