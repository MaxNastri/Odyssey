#pragma once
#include "Resource.h"
#include "RenderPasses.h"

namespace Odyssey
{
	class Mesh;
	class Shader;
	class VulkanPushDescriptors;

	struct RenderSubPassData
	{
		Camera* Camera;
		uint32_t CameraIndex;
	};

	class RenderSubPass
	{
	public:
		virtual void Setup() { }
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) { }
	};

	class OpaqueSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		std::shared_ptr<VulkanPushDescriptors> m_PushDescriptors;
	};

	class DebugSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		ResourceID m_GraphicsPipeline;
		std::shared_ptr<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID m_DescriptorLayout;
	};

	class SkyboxSubPass : public RenderSubPass
	{
	public:
		virtual void Setup() override;
		virtual void Execute(RenderPassParams& params, RenderSubPassData& subPassData) override;

	private:
		ResourceID m_GraphicsPipeline;
		ResourceID m_DescriptorLayout;
		std::shared_ptr<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID uboID;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Mesh> m_CubeMesh;
		inline static std::string SkyboxShaderPath = "Resources/Shaders/Skybox.asset";
		inline static std::string CubeMeshPath = "Resources/Meshes/Cube.asset";
	};
}