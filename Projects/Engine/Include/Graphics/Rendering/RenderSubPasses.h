#pragma once
#include "Resource.h"
#include "RenderPasses.h"

namespace Odyssey
{
	class VulkanPushDescriptors;

	struct RenderSubPassData
	{
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
}