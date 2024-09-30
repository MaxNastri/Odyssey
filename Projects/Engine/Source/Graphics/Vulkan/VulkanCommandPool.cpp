#include "VulkanCommandPool.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
    VulkanCommandPool::VulkanCommandPool(ResourceID id)
        : Resource(id)
    {
    }

    VulkanCommandPool::VulkanCommandPool(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanQueueType queue)
        : Resource(id)
    {
        m_Context = context;
        uint32_t queueIndex = m_Context->GetPhysicalDevice()->GetFamilyIndex(queue);

        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = queueIndex;
        VkResult err = vkCreateCommandPool(m_Context->GetDevice()->GetLogicalDevice(), &info, allocator, &commandPool);
        if (!check_vk_result(err))
        {
            Logger::LogError("(cpool 1)");
        }
    }

    ResourceID VulkanCommandPool::AllocateBuffer()
    {
        // Allocate a new command buffer
        ResourceID commandBuffer = ResourceManager::Allocate<VulkanCommandBuffer>(m_ResourceID);
        commandBuffers.push_back(commandBuffer);

        return commandBuffer;
    }

    void VulkanCommandPool::ReleaseBuffer(ResourceID commandBuffer)
    {
        for (int i = 0; i < commandBuffers.size(); i++)
        {
            if (commandBuffers[i] == commandBuffer)
            {
                ResourceManager::Destroy(commandBuffer);
                commandBuffers.erase(commandBuffers.begin() + i);
                break;
            }
        }
    }

    void VulkanCommandPool::Reset()
    {
        VkResult err = vkResetCommandPool(m_Context->GetDevice()->GetLogicalDevice(), commandPool, 0);
        if (!check_vk_result(err))
        {
            Logger::LogError("(cpool 2)");
        }
    }

    void VulkanCommandPool::Destroy()
    {
        VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
        std::shared_ptr<VulkanCommandPool> pool = ResourceManager::GetResource<VulkanCommandPool>(m_ResourceID);

        // Destroy each of our allocated command buffers
        for (auto& commandBuffer : commandBuffers)
        {
            ResourceManager::Destroy(commandBuffer);
        }

        // Destroy the command pool
        vkDestroyCommandPool(device, commandPool, allocator);
        commandPool = VK_NULL_HANDLE;
        commandBuffers.clear();
    }
}