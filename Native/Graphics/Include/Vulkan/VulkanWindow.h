#pragma once
#include "VulkanGlobals.h"
#include "VulkanSwapchain.h"
#include "VulkanSurface.h"
#include "VulkanFrame.h"
#include "Window.h"
#include <vulkan/vulkan.h>

VK_FWD_DECLARE(VkCommandBuffer)

namespace Odyssey
{
	class VulkanCommandBuffer;
	class VulkanContext;
	class VulkanRenderPass;
	class VulkanQueue;

	class VulkanWindow
	{
	public:
		VulkanWindow(std::shared_ptr<VulkanContext> context);

	public:
		bool Update();
		void PreRender();
		bool BeginFrame(VulkanFrame*& currentFrame);
		void UpdateFrameIndex(uint32_t imageCout);
		void SetRenderPass(VulkanSwapchain* swapchain, std::shared_ptr<VulkanRenderPass> renderPass);
		void Resize(VulkanSwapchain* swapchain);
		const VkSemaphore* GetRenderComplete();
		const VkSemaphore* GetImageAcquired();

	public:
		VulkanSurface* GetSurface() { return surface.get(); }
		Window* GetWindow() { return window.get(); }
		uint32_t GetFrameIndex() { return frameIndex; }
	private:
		void SetupFrameData(VulkanSwapchain* swapchain);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::unique_ptr<VulkanSurface> surface;
		std::unique_ptr<Window> window;

	private: // Frame data
		std::shared_ptr<VulkanRenderPass> frameBufferPass;
		std::vector<VulkanFrame> frames;
		uint32_t frameIndex = 0;
	};
}