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

	public:
		void AddRenderPass(std::shared_ptr<RenderPass> renderPass) { renderPasses.push_back(renderPass); }
	public:
		std::shared_ptr<VulkanImgui> GetImGui() { return imgui; }

	private:
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;

	private: // Commands
		std::vector<ResourceHandle<VulkanCommandPool>> commandPools;
		std::vector<ResourceHandle<VulkanCommandBuffer>> commandBuffers;

	private: // Draws
		std::vector<std::shared_ptr<RenderScene>> renderScenes;
		std::vector<std::shared_ptr<RenderPass>> renderPasses;

		std::shared_ptr<PerFrameRenderingData> renderingData;

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