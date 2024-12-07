#pragma once
#include "glm.h"
#include "Enums.h"
#include "Resource.h"
#include "Drawcall.h"
#include "Ref.h"

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

	struct SceneData
	{
		glm::vec4 ViewPosition;
		mat4 View;
		glm::mat4 ViewProjection;
		glm::mat4 LightViewProj;
	};

	struct ObjectUniformData
	{
		glm::mat4 world;
		glm::mat3 InverseWorld;
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
		SetPass(Ref<Material> material, ResourceID descriptorLayout);

	public:
		void SetMaterial(Ref<Material> material, ResourceID descriptorLayout);

	public:
		ResourceID GraphicsPipeline;
		std::map<ShaderType, ResourceID> Shaders;
		ResourceID ColorTexture;
		ResourceID NormalTexture;
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
		// Uniform structs
		SceneData sceneData;
		ObjectUniformData objectData;
		SkinningData SkinningData;
		LightingData LightingData;

		// Scene objects
		Camera* m_MainCamera = nullptr;
		ResourceID SkyboxCubemap;
		std::vector<SetPass> setPasses;
		std::vector<GUID> ParticleEmitters;

		// Scene uniform buffers
		std::vector<ResourceID> sceneDataBuffers;
		std::vector<ResourceID> perObjectUniformBuffers;
		std::vector<ResourceID> skinningBuffers;
		ResourceID LightingBuffer;
		ResourceID m_DescriptorLayout;

		uint32_t m_NextUniformBuffer = 0;
		uint32_t m_NextCameraBuffer = 0;
		const uint32_t Max_Uniform_Buffers = 128;
		inline static constexpr uint32_t MAX_CAMERAS = 12;
		inline static constexpr uint32_t MAX_LIGHTS = 16;
	};
}