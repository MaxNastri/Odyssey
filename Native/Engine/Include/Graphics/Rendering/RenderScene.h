#pragma once
#include "glm.h"
#include "ResourceHandle.h"
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

	struct UBOMatrices
	{
		glm::mat4 world;
		glm::mat4 inverseView;
		glm::mat4 proj;
	};

	struct SetPass
	{
	public:
		SetPass() = default;
		SetPass(ResourceHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout);

	public:
		void SetMaterial(ResourceHandle<Material> material, ResourceHandle<VulkanDescriptorLayout> descriptorLayout);

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

	public:
		void ConvertScene(Scene* scene);
		void ClearSceneData();

	private:
		void SetupCameraData(Scene* scene);
		void SetupDrawcalls(Scene* scene);
		bool SetPassCreated(ResourceHandle<Material> material, SetPass* outSetPass);

	public:
		// Uniform buffer for scene matrices
		UBOMatrices uboData;
		ResourceHandle<VulkanBuffer> sceneBuffer;
		ResourceHandle<VulkanDescriptorLayout> descriptorLayout;
		std::vector<ResourceHandle<VulkanDescriptorBuffer>> sceneDescriptorBuffers;

		std::vector<SetPass> setPasses;
	};
}