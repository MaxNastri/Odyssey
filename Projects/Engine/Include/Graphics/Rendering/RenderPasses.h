#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Camera.h"

namespace Odyssey
{
	class Camera;
	class VulkanContext;
	class VulkanCommandBuffer;
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
		std::map<uint8_t, ResourceID> DepthTextures;

	public:
		ResourceID Shadowmap() { return DepthTextures[0]; }
	};

	class RenderPass
	{
	public:
		virtual void BeginPass(RenderPassParams& params) = 0;
		virtual void Execute(RenderPassParams& params) = 0;
		virtual void EndPass(RenderPassParams& params) = 0;

	public:
		void SetCamera(uint8_t camera) { m_Camera = camera; }
		void SetRenderTarget(ResourceID renderTarget) { m_RenderTarget = renderTarget; }
		void SetLayouts(VkImageLayout beginLayout, VkImageLayout endLayout) { m_BeginLayout = beginLayout; m_EndLayout = endLayout; }
		void SetClearValue(glm::vec4 clearValue) { m_ClearValue = clearValue; }

	protected:
		ResourceID m_RenderTarget;
		uint8_t m_Camera = 0;

		glm::vec4 m_ClearValue;
		VkImageLayout m_BeginLayout;
		VkImageLayout m_EndLayout;
	};

	class DepthPass : public RenderPass
	{
	public:
		DepthPass(uint8_t cameraTag = 0);

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	private:
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;

	private:
		inline static constexpr uint32_t Texture_Size = 4096;
	};

	class RenderObjectsPass : public RenderPass
	{
	public:
		RenderObjectsPass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	public:
		void AddDebugSubPass();
		void Add2DSubPass();

	private:
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