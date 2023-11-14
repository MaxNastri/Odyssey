#include "VulkanFrame.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	VulkanFrame::VulkanFrame(VkDevice device, uint32_t queueIndex)
	{
		CreateFence(device);
		CreateSempaphores(device);
		commandPool = std::make_unique<VulkanCommandPool>(device, queueIndex);
		commandBuffer = commandPool->AllocateBuffer(device);
	}

	void VulkanFrame::Destroy(VkDevice device)
	{
		commandPool->Destroy(device);

		vkDestroyFence(device, fence, allocator);
		vkDestroyImageView(device, backbufferView, allocator);
		vkDestroyFramebuffer(device, framebuffer, allocator);
		vkDestroySemaphore(device, imageAcquiredSemaphore, allocator);
		vkDestroySemaphore(device, renderCompleteSemaphore, allocator);

		fence = VK_NULL_HANDLE;
		backbufferView = VK_NULL_HANDLE;
		framebuffer = VK_NULL_HANDLE;
		imageAcquiredSemaphore = VK_NULL_HANDLE;
		renderCompleteSemaphore = VK_NULL_HANDLE;
	}

	void VulkanFrame::SetBackbuffer(VkDevice device, VkImage backbufferImage, VkFormat format)
	{
		backbuffer = backbufferImage;

		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;
		info.components.r = VK_COMPONENT_SWIZZLE_R;
		info.components.g = VK_COMPONENT_SWIZZLE_G;
		info.components.b = VK_COMPONENT_SWIZZLE_B;
		info.components.a = VK_COMPONENT_SWIZZLE_A;

		VkImageSubresourceRange image_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		info.subresourceRange = image_range;
		info.image = backbuffer;

		VkResult err = vkCreateImageView(device, &info, allocator, &backbufferView);
		check_vk_result(err);
	}

	void VulkanFrame::CreateFramebuffer(VkDevice device, VkRenderPass renderPass, int width, int height)
	{
		VkImageView attachment[1]
		{
			backbufferView
		};
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = width;
		info.height = height;
		info.layers = 1;
		VkResult err = vkCreateFramebuffer(device, &info, allocator, &framebuffer);
		check_vk_result(err);
	}

	void VulkanFrame::CreateFence(VkDevice device)
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkResult err = vkCreateFence(device, &info, allocator, &fence);
		check_vk_result(err);
	}

	void VulkanFrame::CreateSempaphores(VkDevice device)
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult err = vkCreateSemaphore(device, &info, allocator, &imageAcquiredSemaphore);
		check_vk_result(err);

		err = vkCreateSemaphore(device, &info, allocator, &renderCompleteSemaphore);
		check_vk_result(err);
	}
}