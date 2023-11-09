#pragma once
#ifndef VULKAN_CALLBACKS_H
#define VULKAN_CALLBACKS_H
#include <vulkan/vulkan.h>
#include <Log.h>
#include <format>

namespace Odyssey::Graphics
{
    static VkAllocationCallbacks* allocator = nullptr;
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
    {
        Framework::Log::Error(std::format("[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", (uint64_t)objectType, pMessage));
        return VK_FALSE;
    }

    static void check_vk_result(VkResult err)
    {
        if (err == 0)
        {
            return;
        }

        Framework::Log::Error(std::format("[vulkan] Error: VkResult = %d\n", (uint64_t)err));

        if (err < 0)
        {
            abort();
        }
    }
}
#endif
