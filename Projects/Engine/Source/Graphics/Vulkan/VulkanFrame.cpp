#include "VulkanFrame.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderTexture.h"
#include "VulkanImage.h"

namespace Odyssey
{
	VulkanFrame::VulkanFrame(std::shared_ptr<VulkanContext> context, ResourceID renderTargetID, VkFormat format)
	{
		m_Context = context;
		m_RenderTexture = renderTargetID;

		CreateFence();
		CreateSempaphores();
	}

	void VulkanFrame::Destroy()
	{
		VkDevice vkDevice = m_Context->GetDeviceVK();

		vkDestroyFence(vkDevice, fence, allocator);
		vkDestroySemaphore(vkDevice, imageAcquiredSemaphore, allocator);
		vkDestroySemaphore(vkDevice, renderCompleteSemaphore, allocator);

		fence = VK_NULL_HANDLE;
		imageAcquiredSemaphore = VK_NULL_HANDLE;
		renderCompleteSemaphore = VK_NULL_HANDLE;
	}

	void VulkanFrame::SetRenderTarget(ResourceID renderTargetID, uint32_t imageIndex)
	{
		m_RenderTexture = renderTargetID;
		m_ImageIndex = imageIndex;
	}

	void VulkanFrame::CreateFence()
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkResult err = vkCreateFence(m_Context->GetDeviceVK(), &info, allocator, &fence);
		if (!check_vk_result(err))
		{
			Logger::LogError("(frame 1)");
		}
	}

	void VulkanFrame::CreateSempaphores()
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult err = vkCreateSemaphore(m_Context->GetDeviceVK(), &info, allocator, &imageAcquiredSemaphore);
		if (!check_vk_result(err))
		{
			Logger::LogError("(frame 2)");
		}

		err = vkCreateSemaphore(m_Context->GetDeviceVK(), &info, allocator, &renderCompleteSemaphore);
		if (!check_vk_result(err))
		{
			Logger::LogError("(frame 3)");
		}
	}
}