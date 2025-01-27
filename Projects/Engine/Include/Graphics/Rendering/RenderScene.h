#pragma once
#include "glm.h"
#include "Enums.h"
#include "Resource.h"
#include "Drawcall.h"
#include "Ref.h"
#include "BinaryBuffer.h"
#include "Material.h"

namespace Odyssey
{
	class Camera;
	class Material;
	class Mesh;
	class Scene;
	class VulkanGraphicsPipeline;
	class VulkanDescriptorLayout;
	class VulkanTexture;
	class Light;

	struct SceneData
	{
		glm::vec4 ViewPosition;
		mat4 View;
		mat4 Projection;
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

	struct alignas(16) MaterialData
	{
		float4 EmissiveColor;
		float AlphaClip;
	};

	struct alignas(16) SceneLight
	{
	public:
		float4 Position = float4(0.0f);
		float4 Direction = float4(0.0f);
		float4 Color = float4(0.0f);
		uint32_t Type = 0;
		float Intensity = 0.0f;
		float Range = 1.0f;
	};

	struct alignas(16) LightingData
	{
	public:
		float4 AmbientColor = float4(0.0f);
		std::array<SceneLight, 16> SceneLights;
		uint32_t LightCount = 0;
	};

	struct SetPass
	{
	public:
		SetPass() = default;

	public:
		void SetMaterial(Ref<Material> material, bool skinned);

	public:
		std::map<std::string, ShaderBinding> ShaderBindings;
		std::map<std::string, Ref<Texture2D>> Textures;

		ResourceID GraphicsPipeline;
		ResourceID MaterialBuffer;
		std::map<ShaderType, ResourceID> Shaders;
		RenderQueue RenderQueue;
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

		void SetSceneData(uint8_t cameraTag);

	public:
		Camera* GetCamera(uint8_t cameraTag);
		mat4 GetShadowLightMatrix() { return m_ShadowLightMatrix; }
		bool HasMainCamera() { return m_MainCamera != nullptr; }

	private:
		void SetupDrawcalls(Scene* scene);

	public:
		// Scene objects
		Camera* m_MainCamera = nullptr;
		Light* m_ShadowLight = nullptr;
		mat4 m_ShadowLightMatrix;

		std::map<uint8_t, Camera*> m_Cameras;
		ResourceID SkyboxCubemap;
		std::map<RenderQueue, std::vector<SetPass>> SetPasses;
		std::vector<SpriteDrawcall> SpriteDrawcalls;
		std::map<GUID, size_t> m_GUIDToSetPass;

		std::vector<GUID> ParticleEmitters;

		// Scene uniform buffers
		std::vector<ResourceID> sceneDataBuffers;
		std::vector<ResourceID> perObjectUniformBuffers;
		std::vector<ResourceID> skinningBuffers;
		ResourceID LightingBuffer;

		uint32_t m_NextUniformBuffer = 0;
		uint32_t m_NextMaterialBuffer = 0;

		const uint32_t Max_Uniform_Buffers = 256;
		inline static constexpr uint32_t MAX_CAMERAS = 12;
		inline static constexpr uint32_t MAX_LIGHTS = 16;
	};
}