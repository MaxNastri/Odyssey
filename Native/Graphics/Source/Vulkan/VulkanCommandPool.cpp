#include "VulkanCommandPool.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
    VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueIndex)
    {
        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = queueIndex;
        VkResult err = vkCreateCommandPool(device, &info, allocator, &commandPool);
        check_vk_result(err);
    }

    VkCommandBuffer VulkanCommandPool::AllocateBuffer(VkDevice device)
    {
        int cmdIndex = (int)commandBuffers.size();
        commandBuffers.push_back(nullptr);

        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = commandPool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        VkResult err = vkAllocateCommandBuffers(device, &info, &(commandBuffers[cmdIndex]));
        check_vk_result(err);

        return commandBuffers[cmdIndex];
    }

    void VulkanCommandPool::Reset(VkDevice device)
    {
        VkResult err = vkResetCommandPool(device, commandPool, 0);
        check_vk_result(err);
    }

    void VulkanCommandPool::Destroy(VkDevice device)
    {
        vkFreeCommandBuffers(device, commandPool, (uint32_t)commandBuffers.size(), commandBuffers.data());
        vkDestroyCommandPool(device, commandPool, allocator);
        commandPool = VK_NULL_HANDLE;
        commandBuffers.clear();
    }
}