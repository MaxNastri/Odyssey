#include "VulkanCommandPool.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

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
        check_vk_result(err);
    }

    VulkanCommandBuffer* VulkanCommandPool::AllocateBuffer()
    {
        int cmdIndex = (int)commandBuffers.size();
        commandBuffers.push_back(std::make_unique<VulkanCommandBuffer>(m_Context, this));

        return commandBuffers[cmdIndex].get();
    }

    void VulkanCommandPool::Reset()
    {
        VkResult err = vkResetCommandPool(m_Context->GetDevice()->GetLogicalDevice(), commandPool, 0);
        check_vk_result(err);
    }

    void VulkanCommandPool::Destroy()
    {
        VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
        for (auto& commandBuffer : commandBuffers)
        {
            commandBuffer->Destroy(this);
        }
        vkDestroyCommandPool(device, commandPool, allocator);
        commandPool = VK_NULL_HANDLE;
        commandBuffers.clear();
    }
}