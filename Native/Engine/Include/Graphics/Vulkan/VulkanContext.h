#pragma once
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkInstance)
VK_FWD_DECLARE(VkPhysicalDevice)
VK_FWD_DECLARE(VkDevice)
VK_FWD_DECLARE(VkDebugReportCallbackEXT)
VK_FWD_DECLARE(VkQueue)

struct VkExtensionProperties;

namespace Odyssey
{
	class VulkanCommandBuffer;
	class VulkanCommandPool;
	class VulkanDevice;
	class VulkanPhysicalDevice;
	class VulkanQueue;

	class VulkanContext: public std::enable_shared_from_this<VulkanContext>
	{
	public:
		VulkanContext();
		void Destroy();

	public:
		void SetupResources();
		void SubmitCommandBuffer(ResourceHandle<VulkanCommandBuffer> commandBuffer);

	public:
		VkInstance GetInstance() { return instance; }
		VulkanPhysicalDevice* GetPhysicalDevice() { return physicalDevice.get(); }
		VkPhysicalDevice GetPhysicalDeviceVK();
		VulkanDevice* GetDevice() { return logicalDevice.get(); }
		VkDevice GetDeviceVK();
		VulkanQueue* GetGraphicsQueue();
		const VkQueue GetGraphicsQueueVK();
		ResourceHandle<VulkanCommandPool> GetCommandPool() { return m_CommandPool; }

	private:
		void GatherExtensions();
		void CreateInstance();

		bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension);

	private: // Vulkan objects
		VkInstance instance = nullptr;
		std::shared_ptr<VulkanPhysicalDevice> physicalDevice;
		std::shared_ptr<VulkanDevice> logicalDevice;
		ResourceHandle<VulkanCommandPool> m_CommandPool;
		std::shared_ptr<VulkanQueue> m_GraphicsQueue;

	private: // Extensions
		std::vector<const char*> extensions;
		uint32_t extensionsCount = 0;

	private: // Debugging
		bool enableValidation = true;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	};
}