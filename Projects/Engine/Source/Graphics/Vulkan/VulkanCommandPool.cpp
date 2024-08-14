#include "VulkanCommandPool.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "ResourceHandle.h"

namespace Odyssey
{
    VulkanCommandPool::VulkanCommandPool(std::shared_ptr<VulkanContext> context)
    {
        m_Context = context;
        uint32_t queueIndex = m_Context->GetPhysicalDevice()->GetFamilyIndex(VulkanQueueType::Graphics);

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

    ResourceHandle<VulkanCommandBuffer> VulkanCommandPool::AllocateBuffer()
    {
        ResourceHandle poolHandle = ResourceHandle(m_ID, this);
        ResourceHandle<VulkanCommandBuffer> bufferHandle = ResourceManager::AllocateCommandBuffer(poolHandle);
        commandBuffers.push_back(bufferHandle);

        return bufferHandle;
    }

    void VulkanCommandPool::ReleaseBuffer(ResourceHandle<VulkanCommandBuffer> commandBuffer)
    {
        for (int i = 0; i < commandBuffers.size(); i++)
        {
            if (commandBuffers[i].GetID() == commandBuffer.GetID())
            {
                ResourceHandle poolHandle = ResourceHandle(m_ID, this);
                ResourceManager::DestroyCommandBuffer(commandBuffer, poolHandle);
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
        ResourceHandle poolHandle = ResourceHandle(m_ID, this);
        for (auto& commandBuffer : commandBuffers)
        {
            ResourceManager::DestroyCommandBuffer(commandBuffer, poolHandle);
        }
        vkDestroyCommandPool(device, commandPool, allocator);
        commandPool = VK_NULL_HANDLE;
        commandBuffers.clear();
    }
}