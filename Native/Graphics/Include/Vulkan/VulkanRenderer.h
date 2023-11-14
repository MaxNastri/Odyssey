#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "Window.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanDescriptorPool.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanFrame.h"
#include "VulkanImgui.h"

namespace Odyssey
{
	class GUIElement;
	class VulkanContext;

	class VulkanRenderer
	{
	public:
		VulkanRenderer();

	public:
		bool Update();
		bool Render();

	private:
		void RenderFrame();
		void Present();
		void SetupFrameData();
		void RebuildSwapchain();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();

	private:
		std::unique_ptr<Window> window;
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
		bool rebuildSwapchain = false;

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::unique_ptr<VulkanQueue> graphicsQueue;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::unique_ptr<VulkanSurface> surface;
		std::unique_ptr<VulkanSwapchain> swapchain;
		std::unique_ptr<VulkanRenderPass> renderPass;
		std::unique_ptr<VulkanImgui> imgui;
	};
}