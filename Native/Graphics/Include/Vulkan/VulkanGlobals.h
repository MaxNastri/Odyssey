#pragma once
#ifndef VULKAN_CALLBACKS_H
#define VULKAN_CALLBACKS_H
#include <vulkan/vulkan.h>
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
    };

    // ALLOCATIONS
    static VkAllocationCallbacks* allocator = nullptr;


    // DEBUGGING
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
    {
        Logger::LogError("[vulkan] Debug report: " + std::string(pMessage));
        return VK_FALSE;
    }

    static void check_vk_result(VkResult err)
    {
        if (err == 0)
        {
            return;
        }

        Logger::LogError(std::format("[vulkan] Error: VkResult = %d\n", (uint64_t)err));

        if (err < 0)
        {
            abort();
        }
    }
}
#endif
