#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkRenderPass)

namespace Odyssey
{
	class VulkanDevice;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VkFormat surfaceFormat);

	public:
		void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, int width, int height, VkClearValue clearValue);
		void End(VkCommandBuffer commandBuffer);

	public:
		VkRenderPass GetVK() { return renderPass; }

	private:
		VkRenderPass renderPass;
	};
}