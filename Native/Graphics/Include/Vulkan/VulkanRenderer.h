#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "Window.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanDescriptorPool.h"
#include "VulkanImgui.h"

namespace Odyssey
{
	class GUIElement;
	class VulkanContext;
	class VulkanWindow;
	class VulkanRenderPass;

	class VulkanRenderer
	{
	public:
		VulkanRenderer();

	public:
		bool Update();
		bool Render();

	private:
		void RenderFrame();

	private:
		VulkanImgui::InitInfo CreateImguiInitInfo();

	private: // Vulkan objects
		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanWindow> window;
		std::unique_ptr<VulkanQueue> graphicsQueue;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::shared_ptr<VulkanRenderPass> renderPass;
		std::unique_ptr<VulkanImgui> imgui;
	};
}