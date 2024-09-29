#pragma once
#include "VulkanGlobals.h"
#include "Resource.h"

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

	public:
		VkInstance GetInstance() { return instance; }
		VulkanPhysicalDevice* GetPhysicalDevice() { return physicalDevice.get(); }
		VkPhysicalDevice GetPhysicalDeviceVK();
		VulkanDevice* GetDevice() { return logicalDevice.get(); }
		VkDevice GetDeviceVK();
		VulkanQueue* GetGraphicsQueue();
		VulkanQueue* GetComputeQueue();
		const VkQueue GetGraphicsQueueVK();
		const VkQueue GetComputeQueueVK();
		ResourceID GetGraphicsCommandPool() { return m_GraphicsCommandPool; }
		ResourceID GetComputeCommandPool() { return m_ComputeCommandPool; }

	private:
		void GatherExtensions();
		void CreateInstance();

		bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension);

	private: // Vulkan objects
		VkInstance instance = nullptr;
		std::shared_ptr<VulkanPhysicalDevice> physicalDevice;
		std::shared_ptr<VulkanDevice> logicalDevice;
		ResourceID m_GraphicsCommandPool;
		ResourceID m_ComputeCommandPool;
		std::shared_ptr<VulkanQueue> m_GraphicsQueue;
		std::shared_ptr<VulkanQueue> m_ComputeQueue;

	private: // Extensions
		std::vector<const char*> extensions;
		uint32_t extensionsCount = 0;

	private: // Debugging
		bool enableValidation = true;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	};
}