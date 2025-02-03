#pragma once
#include "Drawcall.h"
#include "Ref.h"
#include "RenderPasses.h"
#include "RenderScene.h"
#include "ResourceManager.h"
#include "volk.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanFrame.h"
#include "VulkanImgui.h"
#include "VulkanShaderModule.h"
#include "VulkanSwapchain.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanFrame;
	class VulkanWindow;

	class VulkanRenderer
	{
	public:
		VulkanRenderer();
		void Destroy();

	public:
		bool Update();
		bool Render();
		bool Present();

	public:
		void AddRenderPass(Ref<RenderPass> renderPass) { m_RenderPasses.push_back(renderPass); }
		void AddImguiPass();
		void CaptureCursor();
		void ReleaseCursor();

	public:
		std::shared_ptr<VulkanImgui> GetImGui() { return m_Imgui; }
		std::shared_ptr<VulkanWindow> GetWindow() { return m_Window; }
		static uint32_t GetFrameIndex() { return s_FrameIndex; }

	private:
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();
		void BuildIrradianceCubemap(RenderPassParams& params);

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> m_Context;
		std::shared_ptr<VulkanWindow> m_Window;

	private: // Commands
		std::vector<ResourceID> m_GraphicsCommandPools;
		std::vector<ResourceID> m_GraphicsCommandBuffers;

	private: // Draws
		std::vector<std::shared_ptr<RenderScene>> m_RenderScenes;
		std::vector<Ref<RenderPass>> m_RenderPasses;
		std::shared_ptr<ImguiPass> m_IMGUIPass;
		std::shared_ptr<PerFrameRenderingData> m_RenderingData;

	private: // IMGUI
		std::shared_ptr<VulkanImgui> m_Imgui;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> m_Swapchain;
		bool m_RebuildSwapchain = false;

	private:
		ResourceID m_BRDFLutTexture;
		ResourceID m_IrradianceCubemap;

	private: // Frame data
		std::vector<VulkanFrame> m_Frames;
		inline static uint32_t s_FrameIndex = 0;
		inline static uint32_t s_PreviousFrame = 0;

	private: // Const
		const float DEFAULT_FONT_SIZE = 18.0f;
	};
}