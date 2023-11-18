#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanDescriptorPool.h"
#include "VulkanImgui.h"
#include "VulkanSwapchain.h"
#include "VulkanFrame.h"
#include "VulkanCommandPool.h"

namespace Odyssey
{
	class GUIElement;
	class VulkanContext;
	class VulkanWindow;
	class VulkanRenderPass;
	class VulkanFrame;

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
		bool BeginFrame(VulkanFrame*& currentFrame);
		void RenderFrame();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();
		void SetupFrameData();

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;
		std::unique_ptr<VulkanQueue> graphicsQueue;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::shared_ptr<VulkanRenderPass> renderPass;
		std::unique_ptr<VulkanImgui> imgui;
		std::vector<std::unique_ptr<VulkanCommandPool>> commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	private: // Swapchain
		std::unique_ptr<VulkanSwapchain> swapchain;
		bool rebuildSwapchain = false;

	private: // Frame data
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}