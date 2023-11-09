#pragma once
#include <vulkan/vulkan.h>

namespace Odyssey::Graphics
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VkDevice device, VkFormat surfaceFormat);

	public:
		void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, int width, int height, VkClearValue clearValue);
		void End(VkCommandBuffer commandBuffer);

	public:
		VkRenderPass GetVK() { return renderPass; }

	private:
		VkRenderPass renderPass;
	};
}