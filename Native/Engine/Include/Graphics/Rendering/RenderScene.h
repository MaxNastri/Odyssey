#pragma once
#include "glm.h"
#include "ResourceHandle.h"
#include "AssetHandle.h"
#include "Drawcall.h"

namespace Odyssey
{
	class Material;
	class Mesh;
	class Scene;
	class VulkanGraphicsPipeline;
	class VulkanDescriptorBuffer;
	class VulkanBuffer;
	class VulkanDescriptorLayout;

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
		SetPass(AssetHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout);

	public:
		void SetMaterial(AssetHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout);

	public:
		ResourceHandle<VulkanGraphicsPipeline> pipeline;
		std::vector<Drawcall> drawcalls;
		uint32_t vertexShaderID = -1;
		uint32_t fragmentShaderID = -1;
	};

	class RenderScene
	{
	public:
		RenderScene();
		void Destroy();

	public:
		void ConvertScene(Scene* scene);
		void ClearSceneData();

	private:
		void SetupCameraData(Scene* scene);
		void SetupDrawcalls(Scene* scene);
		bool SetPassCreated(AssetHandle<Material> material, SetPass* outSetPass);

	public:
		// Data structs
		SceneUniformData sceneData;
		ObjectUniformData objectData;

		// Descriptor buffer for per-scene data
		ResourceHandle<VulkanDescriptorLayout> descriptorLayout;
		ResourceHandle<VulkanBuffer> sceneUniformBuffer;
		std::vector<ResourceHandle<VulkanBuffer>> perObjectUniformBuffers;
		ResourceHandle<VulkanDescriptorBuffer> descriptorBuffer;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 512;
	};
}