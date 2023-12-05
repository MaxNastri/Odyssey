#pragma once
#include "ResourceHandle.h"
#include "glm.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	class VulkanContext;
	class VulkanCommandBuffer;
	class VulkanTexture;
	class VulkanShaderModule;
	class VulkanGraphicsPipeline;
	class VulkanImgui;
	class VulkanImage;
	struct PerFrameRenderingData;

	struct RenderPassParams
	{
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<PerFrameRenderingData> renderingData;
		ResourceHandle<VulkanCommandBuffer> commandBuffer;
	};

	class RenderPass
	{
	public:
		virtual void BeginPass(RenderPassParams& params) = 0;
		virtual void Execute(RenderPassParams& params) = 0;
		virtual void EndPass(RenderPassParams& params) = 0;

	public:
		void SetRenderTexture(ResourceHandle<VulkanTexture> renderTarget);
		void SetRenderTarget(VulkanImage* renderTarget);
		void SetLayouts(VkImageLayout oldLayout, VkImageLayout newLayout) { m_OldLayout = oldLayout; m_NewLayout = newLayout; }
		void SetClearValue(glm::vec4 clearValue) { m_ClearValue = clearValue; }

	protected:
		ResourceHandle<VulkanTexture> m_RenderTexture;
		VulkanImage* m_RenderTarget;
		glm::vec4 m_ClearValue;
		VkImageLayout m_OldLayout;
		VkImageLayout m_NewLayout;
	};

	class OpaquePass : public RenderPass
	{
	public:
		OpaquePass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;
	};

	class ImguiPass : public RenderPass
	{
	public:
		ImguiPass() = default;

	public:
		void SetRenderTarget(VulkanImage* renderTarget);

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