#pragma once
#ifndef VULKAN_CALLBACKS_H
#define VULKAN_CALLBACKS_H
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#include "volk.h"
#include <Logger.h>
#include <format>

// DEFINES
#ifndef VK_FWD_DECLARE
#define VK_FWD_DECLARE(object) \
struct object##_T;\
typedef object##_T* object;
#endif

namespace Odyssey
{
    // TYPES
    enum VulkanQueueType
    {
        Graphics = 0,
        Compute = 1,
        Transfer = 2,
    };

    struct VulkanQueueFamilies
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> computeFamily;
    };

    // ALLOCATIONS
    static VkAllocationCallbacks* allocator = nullptr;


    // DEBUGGING
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
    {
        Logger::LogError("[vulkan] Debug report: " + std::string(pMessage));
        return VK_FALSE;
    }

    static bool check_vk_result(VkResult err)
    {
        if (err == 0)
        {
            return true;
        }

        Logger::LogError("[vulkan] Error: VkResult = " + std::to_string((uint64_t)err));
        return false;
    }
}
#endif
