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

namespace Odyssey::Graphics
{
	class VulkanRenderer
	{
	public:
		VulkanRenderer();

	public:
		void Update();
		void Present();

	private:
		void GatherExtensions();
		void CreateInstance();

		bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension);

	private: // Debug
		bool enableValidation = true;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;

	private: // Extensions
		std::vector<const char*> extensions;
		uint32_t extensionsCount;

	private:
		std::unique_ptr<Window> window;
		std::vector<std::unique_ptr<VulkanFrame>> frames;
		uint32_t frameIndex = 0;
		bool rebuildSwapchain = false;

	private: // Vulkan objects
		VkAllocationCallbacks* allocator = nullptr;
		VkInstance instance = VK_NULL_HANDLE;
		std::unique_ptr<VulkanDevice> device;
		std::unique_ptr<VulkanQueue> graphicsQueue;
		std::unique_ptr<VulkanDescriptorPool> descriptorPool;
		std::unique_ptr<VulkanSurface> surface;
		std::unique_ptr<VulkanSwapchain> swapchain;
		std::unique_ptr<VulkanRenderPass> renderPass;
	};
}