#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkInstance)
VK_FWD_DECLARE(VkDebugReportCallbackEXT)
VK_FWD_DECLARE(VkQueue)

struct VkExtensionProperties;

namespace Odyssey
{
	class VulkanDevice;
	class VulkanPhysicalDevice;
	class VulkanCommandPool;
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
		VulkanDevice* GetDevice() { return logicalDevice.get(); }
		VulkanQueue* GetGraphicsQueue();
		const VkQueue GetGraphicsQueueVK();
		VulkanCommandPool* GetCommandPool() { return m_CommandPool.get(); }

	private:
		void GatherExtensions();
		void CreateInstance();

		bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension);

	private: // Vulkan objects
		VkInstance instance = nullptr;
		std::shared_ptr<VulkanPhysicalDevice> physicalDevice;
		std::shared_ptr<VulkanDevice> logicalDevice;
		std::shared_ptr<VulkanCommandPool> m_CommandPool;
		std::map<VulkanQueueType, std::shared_ptr<VulkanQueue>> m_Queues;

	private: // Extensions
		std::vector<const char*> extensions;
		uint32_t extensionsCount = 0;

	private: // Debugging
		bool enableValidation = true;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	};
}