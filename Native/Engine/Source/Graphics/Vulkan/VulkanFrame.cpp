#include "VulkanFrame.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanImage.h"

namespace Odyssey
{
	VulkanFrame::VulkanFrame(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanImage> renderTarget, VkFormat format)
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

	VkImage VulkanFrame::GetRenderTargetVK()
	{
		return m_RenderTarget->GetImage();
	}

	VkImageView VulkanFrame::GetRenderTargetViewVK()
	{
		return m_RenderTarget->GetImageView();
	}

	void VulkanFrame::CreateFence()
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkResult err = vkCreateFence(m_Context->GetDeviceVK(), &info, allocator, &fence);
		check_vk_result(err);
	}

	void VulkanFrame::CreateSempaphores()
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult err = vkCreateSemaphore(m_Context->GetDeviceVK(), &info, allocator, &imageAcquiredSemaphore);
		check_vk_result(err);

		err = vkCreateSemaphore(m_Context->GetDeviceVK(), &info, allocator, &renderCompleteSemaphore);
		check_vk_result(err);
	}
}