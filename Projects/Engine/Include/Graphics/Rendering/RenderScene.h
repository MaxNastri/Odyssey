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
		glm::vec4 ViewPosition;
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

	struct alignas(16) SceneLight
	{
	public:
		glm::vec4 Position;
		glm::vec4 Direction;
		glm::vec4 Color;
		uint32_t Type;
		float Intensity;
		float Range;
	};

	struct alignas(16) LightingData
	{
	public:
		glm::vec4 AmbientColor;
		std::array<SceneLight, 16> SceneLights;
		uint32_t LightCount;
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
		LightingData LightingData;
		Camera* m_MainCamera = nullptr;

		ResourceID SkyboxCubemap;

		// Descriptor buffer for per-scene data
		std::vector<ResourceID> cameraDataBuffers;
		std::vector<ResourceID> perObjectUniformBuffers;
		std::vector<ResourceID> skinningBuffers;
		ResourceID LightingBuffer;

		ResourceID m_DescriptorLayout;

		std::vector<SetPass> setPasses;
		uint32_t m_NextUniformBuffer = 0;
		uint32_t m_NextCameraBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
		inline static constexpr uint32_t MAX_CAMERAS = 12;
		inline static constexpr uint32_t MAX_LIGHTS = 16;
	};
}