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
		std::map<uint8_t, ResourceID> ColorTextures;
		ResourceID BRDFLutTexture;

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

	protected:
		void BeginRendering(RenderPassParams& params);

	protected:
		struct AttachmentInfo
		{
			VkImageLayout BeginLayout;
			VkImageLayout EndLayout;
			VkAttachmentLoadOp LoadOp;
			VkAttachmentStoreOp StoreOp;
			VkClearValue ClearValue;
		};

		ResourceID m_RenderTarget;
		uint8_t m_Camera = 0;

		AttachmentInfo m_ColorAttachment;
		AttachmentInfo m_DepthAttachment;
	};

	class BRDFLutPass : public RenderPass
	{
	public:
		BRDFLutPass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	private:
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;

	private:
		inline static constexpr uint32_t Texture_Size = 512;
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
		void CreateRenderTarget(uint32_t width, uint32_t height);

	private:
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;

	private:
		uint32_t m_Width, m_Height;

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

	class TransparentObjectsPass : public RenderPass
	{
	public:
		TransparentObjectsPass();

	public:
		virtual void BeginPass(RenderPassParams& params) override;
		virtual void Execute(RenderPassParams& params) override;
		virtual void EndPass(RenderPassParams& params) override;

	private:
		std::vector<std::shared_ptr<RenderSubPass>> m_SubPasses;
		ResourceID m_CameraColorTexture;
	};

	class ImguiPass : public RenderPass
	{
	public:
		ImguiPass();

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