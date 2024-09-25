#pragma once
#include "glm.h"
#include "Enums.h"
#include "Resource.h"
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
		glm::mat4 ViewProjection;
	};

	struct ObjectUniformData
	{
		glm::mat4 world;
	};

	struct SkinningData
	{
		std::array<glm::mat4, 128> Bones;

		SkinningData()
		{
			for (size_t i = 0; i < Bones.size(); i++)
			{
				Bones[i] = glm::identity<mat4>();
			}
		}

		void SetBindposes(std::vector<glm::mat4> bindposes)
		{
			assert(bindposes.size() <= Bones.size());
			for (size_t i = 0; i < bindposes.size(); i++)
			{
				Bones[i] = bindposes[i];
			}
		}
	};

	struct SetPass
	{
	public:
		SetPass() = default;
		SetPass(std::shared_ptr<Material> material, ResourceID descriptorLayout);

	public:
		void SetMaterial(std::shared_ptr<Material> material, ResourceID descriptorLayout);

	public:
		ResourceID GraphicsPipeline;
		std::map<ShaderType, ResourceID> Shaders;
		ResourceID Texture;
		std::vector<Drawcall> Drawcalls;
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
		SkinningData SkinningData;
		Camera* m_MainCamera = nullptr;

		// Descriptor buffer for per-scene data
		std::vector<ResourceID> cameraDataBuffers;
		std::vector<ResourceID> perObjectUniformBuffers;
		std::vector<ResourceID> skinningBuffers;

		ResourceID m_DescriptorLayout;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		uint32_t m_NextCameraBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
		inline static constexpr uint32_t MAX_CAMERAS = 12;
	};
}