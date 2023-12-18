#include "VulkanFrame.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanImage.h"

namespace Odyssey
{
	VulkanFrame::VulkanFrame(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanImage> renderTarget, VkFormat format)
	{
		m_Context = context;
		m_RenderTarget = renderTarget;

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

	void VulkanFrame::SetRenderTarget(ResourceHandle<VulkanImage> renderTarget, uint32_t imageIndex)
	{
		m_RenderTarget = renderTarget;
		m_ImageIndex = imageIndex;
	}

	VkImage VulkanFrame::GetRenderTargetVK()
	{
		return m_RenderTarget.Get()->GetImage();
	}

	VkImageView VulkanFrame::GetRenderTargetViewVK()
	{
		return m_RenderTarget.Get()->GetImageView();
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