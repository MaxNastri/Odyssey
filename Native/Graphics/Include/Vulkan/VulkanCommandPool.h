#pragma once
#include "VulkanGlobals.h"
#include "VulkanCommandBuffer.h"

VK_FWD_DECLARE(VkCommandPool)

namespace Odyssey
{
	class VulkanContext;
	class VulkanDevice;

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;
		VulkanCommandPool(std::shared_ptr<VulkanContext> context);

	public:
		VulkanCommandBuffer* AllocateBuffer();
		void ReleaseBuffer(VulkanCommandBuffer* commandBuffer);

		void Reset();
		void Destroy();

	public:
		VkCommandPool GetCommandPool() { return commandPool; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkCommandPool commandPool;
		std::vector<std::unique_ptr<VulkanCommandBuffer>> commandBuffers;
	};
}