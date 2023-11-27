#pragma once
#include "volk.h"
#include "VulkanDevice.h"
#include "VulkanImgui.h"
#include "VulkanSwapchain.h"
#include "VulkanFrame.h"
#include "VulkanCommandPool.h"
#include "VulkanShader.h"
#include "ResourceManager.h"
#include "Drawcall.h"
#include "RenderScene.h"
#include "RenderPasses.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanFrame;
	class VulkanIndexBuffer;
	class VulkanTexture;
	class VulkanVertexBuffer;
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

	private:
		void BuildRenderGraph();
		void CreateRenderPasses();
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();
		void SetupDrawData();

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;

	private: // Commands
		std::vector<ResourceHandle<VulkanCommandPool>> commandPools;
		std::vector<ResourceHandle<VulkanCommandBuffer>> commandBuffers;


	private: // Draws
		std::vector<std::shared_ptr<RenderScene>> renderScenes;
		std::vector<std::unique_ptr<RenderPass>> renderPasses;
		ResourceHandle<VulkanGraphicsPipeline> graphicsPipeline;

		std::shared_ptr<PerFrameRenderingData> renderingData;

	private: // Render texture stuff
		ResourceHandle<VulkanTexture> renderTexture;
		VkDescriptorSet rtSet;

	private: // IMGUI
		std::shared_ptr<VulkanImgui> imgui;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> swapchain;
		bool rebuildSwapchain = false;

	private: // Frame data
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}