#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <glfw3.h>
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

namespace Odyssey
{
	VulkanContext::VulkanContext()
	{
		VkResult result = volkInitialize();
		GatherExtensions();
		CreateInstance();
		volkLoadInstance(instance);

		physicalDevice = std::make_shared<VulkanPhysicalDevice>(instance);
		logicalDevice = std::make_shared<VulkanDevice>(physicalDevice.get());
		GetSupportedMSAA();
	}

	void VulkanContext::Destroy()
	{
		ResourceManager::Destroy(m_GraphicsCommandPool);
		ResourceManager::Destroy(m_ComputeCommandPool);
	}

	void VulkanContext::SetupResources()
	{
		m_GraphicsCommandPool = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Graphics);
		m_ComputeCommandPool = ResourceManager::Allocate<VulkanCommandPool>(VulkanQueueType::Compute);
		m_GraphicsQueue = std::make_shared<VulkanQueue>(VulkanQueueType::Graphics, VulkanContext::shared_from_this());
		m_ComputeQueue = std::make_shared<VulkanQueue>(VulkanQueueType::Compute, VulkanContext::shared_from_this());
	}

	VkPhysicalDevice VulkanContext::GetPhysicalDeviceVK()
	{
		return physicalDevice->GetPhysicalDevice();
	}

	VkDevice VulkanContext::GetDeviceVK()
	{
		return logicalDevice->GetLogicalDevice();
	}

	VulkanQueue* VulkanContext::GetGraphicsQueue()
	{
		return m_GraphicsQueue.get();
	}

	VulkanQueue* VulkanContext::GetComputeQueue()
	{
		return m_ComputeQueue.get();
	}

	const VkQueue VulkanContext::GetGraphicsQueueVK()
	{
		return m_GraphicsQueue->queue;
	}

	const VkQueue VulkanContext::GetComputeQueueVK()
	{
		return m_ComputeQueue->queue;
	}

	void VulkanContext::GatherExtensions()
	{
		if (glfwInit())
		{
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
			for (uint32_t i = 0; i < extensionsCount; ++i)
			{
				extensions.push_back(glfwExtensions[i]);
			}
		}
	}

	void VulkanContext::CreateInstance()
	{
		VkResult err;

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Odyssey";
		appInfo.pEngineName = "Odyssey";
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		// Enumerate available extensions
		uint32_t properties_count;
		std::vector<VkExtensionProperties> properties;
		vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.data());
		if (!check_vk_result(err))
		{
			Log::Error("(context 1)");
		}

		// Enable required extensions
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		}
#endif

		if (enableValidation)
		{
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;
			extensions.push_back("VK_EXT_debug_report");
		}

		// Create Vulkan Instance
		create_info.pApplicationInfo = &appInfo;
		create_info.enabledExtensionCount = (uint32_t)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();
		err = vkCreateInstance(&create_info, allocator, &instance);
		if (!check_vk_result(err))
		{
			Log::Error("(context 2)");
		}

		// Setup the debug report callback
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		assert(vkCreateDebugReportCallbackEXT != nullptr);

		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = nullptr;
		err = vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debugReport);
		if (!check_vk_result(err))
		{
			Log::Error("(context 3)");
		}
	}

	void VulkanContext::GetSupportedMSAA()
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice->GetPhysicalDevice(), &properties);

		VkSampleCountFlags count = properties.limits.framebufferColorSampleCounts &
			properties.limits.framebufferDepthSampleCounts;

		if (count & VK_SAMPLE_COUNT_64_BIT)
			m_SampleCount = 64;
		else if (count & VK_SAMPLE_COUNT_32_BIT)
				m_SampleCount = 32;
		else if (count & VK_SAMPLE_COUNT_16_BIT)
			m_SampleCount = 16;
		else if (count & VK_SAMPLE_COUNT_8_BIT)
			m_SampleCount = 8;
		else if (count & VK_SAMPLE_COUNT_4_BIT)
			m_SampleCount = 4;
		else if (count & VK_SAMPLE_COUNT_2_BIT)
			m_SampleCount = 2;
	}

	bool VulkanContext::IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension)
	{
		for (const VkExtensionProperties& p : properties)
		{
			if (strcmp(p.extensionName, extension) == 0)
			{
				return true;
			}
		}

		return false;
	}
}