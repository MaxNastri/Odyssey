#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Camera.h"

namespace Odyssey
{
	class Camera;
	class VulkanContext;
	class VulkanCommandBuffer;
	class VulkanRenderTexture;
	class VulkanShaderModule;
	class VulkanGraphicsPipeline;
	class VulkanImgui;
	class VulkanImage;
	class VulkanPushDescriptors;
	struct PerFrameRenderingData;
	class RenderSubPass;

	struct RenderPassParams
	{
	public:
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<PerFrameRenderingData> renderingData;
		ResourceID GraphicsCommandBuffer;
		ResourceID FrameTexture;
		ResourceID Shadowmap;
	};

	class RenderPass
	{
	public:
		virtual void BeginPass(RenderPassParams& params) = 0;
		virtual void Execute(RenderPassParams& params) = 0;
		virtual void EndPass(RenderPassParams& params) = 0;

	public:
		void SetColorRenderTexture(ResourceID colorRT);
		void SetDepthRenderTexture(ResourceID depthRT);
		void SetLayouts(VkImageLayout beginLayout, VkImageLayout endLayout) { m_BeginLayout = beginLayout; m_EndLayout = endLayout; }
		void SetClearValue(glm::vec4 clearValue) { m_ClearValue = clearValue; }

	protected:
		ResourceID m_ColorRT;
		ResourceID m_DepthRT;

		glm::vec4 m_ClearValue;
		// Starting layout
		// Ending layout
		VkImageLayout m_BeginLayout;
		VkImageLayout m_EndLayout;
	};

	class ShadowPass : public RenderPass
	{
	public:
		ShadowPass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	private:
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;

	private:
		inline static constexpr uint32_t Shadowmap_Size = 2048;
	};

	class OpaquePass : public RenderPass
	{
	public:
		OpaquePass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	public:
		void SetCamera(Camera* camera) { m_Camera = camera; }
		void AddDebugSubPass();

	private:
		Camera* m_Camera = nullptr;
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;
	};

	class ImguiPass : public RenderPass
	{
	public:
		ImguiPass() = default;

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	public:
		void SetImguiState(std::shared_ptr<VulkanImgui> imgui);

	private:
		std::shared_ptr<VulkanImgui> m_Imgui;
	};
}