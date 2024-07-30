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

	struct CameraUniformData
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
		uint64_t vertexShaderID = (uint64_t)(-1);
		uint64_t fragmentShaderID = (uint64_t)(-1);
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

		uint32_t SetCameraData(Camera* camera);
		bool HasMainCamera() { return m_MainCamera != nullptr; }

	private:
		void SetupDrawcalls(Scene* scene);

	public:
		// Data structs
		CameraUniformData cameraData;
		ObjectUniformData objectData;
		Camera* m_MainCamera = nullptr;

		// Descriptor buffer for per-scene data
		ResourceHandle<VulkanDescriptorLayout> uboLayout;
		std::vector<ResourceHandle<VulkanUniformBuffer>> cameraDataBuffers;
		std::vector<ResourceHandle<VulkanUniformBuffer>> perObjectUniformBuffers;

		// Descriptor buffer for per-object sampler
		ResourceHandle<VulkanDescriptorLayout> m_SamplerLayout;

		std::vector<ResourceHandle<VulkanDescriptorLayout>> m_Layouts;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		uint32_t m_NextCameraBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
		inline static constexpr uint32_t MAX_CAMERAS = 4;
	};
}