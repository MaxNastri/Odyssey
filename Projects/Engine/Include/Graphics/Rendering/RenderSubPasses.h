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
		std::shared_ptr<Shader> m_Shader;
		ResourceID m_GraphicsPipeline;
		std::shared_ptr<VulkanPushDescriptors> m_PushDescriptors;
		ResourceID m_DescriptorLayout;

	private:
		inline static const GUID& s_DebugShaderGUID = 3857300505190494605;
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
		inline static const GUID& s_SkyboxShaderGUID = 12373133592092994291;
		inline static std::string CubeMeshPath = "Resources/Meshes/Cube.asset";
	};
}