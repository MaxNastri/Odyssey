#include "VulkanFrame.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	VulkanFrame::VulkanFrame(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
	{
		uint32_t graphicsIndex = physicalDevice->GetFamilyIndex(VulkanQueueType::Graphics);
		VkDevice vkDevice = device->GetLogicalDevice();
		CreateFence(vkDevice);
		CreateSempaphores(vkDevice);
	}

	void VulkanFrame::Destroy(VulkanDevice* device)
	{
		VkDevice vkDevice = device->GetLogicalDevice();

		vkDestroyFence(vkDevice, fence, allocator);
		vkDestroyImageView(vkDevice, backbufferView, allocator);
		vkDestroySemaphore(vkDevice, imageAcquiredSemaphore, allocator);
		vkDestroySemaphore(vkDevice, renderCompleteSemaphore, allocator);

		fence = VK_NULL_HANDLE;
		backbufferView = VK_NULL_HANDLE;
		imageAcquiredSemaphore = VK_NULL_HANDLE;
		renderCompleteSemaphore = VK_NULL_HANDLE;
	}

	void VulkanFrame::SetBackbuffer(VulkanDevice* device, VkImage backbufferImage, VkFormat format)
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

		VkResult err = vkCreateImageView(device->GetLogicalDevice(), &info, allocator, &backbufferView);
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